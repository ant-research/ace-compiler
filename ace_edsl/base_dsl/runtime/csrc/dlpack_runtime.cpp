#include "dlpack/dlpack.h"
#include <Python.h>
#include <algorithm>
#include <chrono>
#include <cstring>
#include <iostream>
#include <nanobind/nanobind.h>
#include <nanobind/ndarray.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/vector.h>
#include <variant>

namespace nb = nanobind;

enum class TensorFormat { compact = 0, non_compact = 1 };

struct MemRefDescriptor {
  void *ptr;
  // {$nv-internal-release begin}
  // cutegen only support int32_t for now, so we use int32_t for shape_stride
  // {$nv-internal-release end}
  int32_t shape_stride[];
};

class DynVal {
public:
  DynVal(int64_t v, int64_t div = 1) : value_(v), div_(div) {}
  ~DynVal() = default;

  int64_t value() const { return value_; }
  int64_t div() const { return div_; }

  // String representation (equivalent to Python __str__)
  std::string toString() const {
    if (div_ > 1) {
      return "?{div=" + std::to_string(div_) + "}";
    }
    return "?";
  }

  // Debug representation (equivalent to Python __repr__)
  std::string toRepr() const {
    return toString() + "=" + std::to_string(value_);
  }

  DynVal operator*(const DynVal &other) const {
    return DynVal(value_ * other.value_, div_ * other.div_);
  }

  DynVal operator*(int64_t other) const {
    return DynVal(value_ * other, div_ * other);
  }

  friend DynVal operator*(int64_t lhs, const DynVal &rhs) {
    return DynVal(lhs * rhs.value_, lhs * rhs.div_);
  }

private:
  int64_t value_; // Actual value
  int64_t div_;   // Divisibility information
};
// Type alias for the variant that can hold either an int64_t or DynVal
using ShapeElement = std::variant<int64_t, DynVal>;

ShapeElement operator*(const ShapeElement &lhs, const ShapeElement &rhs) {
  // If both operands are static
  if (std::holds_alternative<int64_t>(lhs) &&
      std::holds_alternative<int64_t>(rhs)) {
    return std::get<int64_t>(lhs) * std::get<int64_t>(rhs);
  }

  // If at least one operand is dynamic (DynVal)
  if (std::holds_alternative<DynVal>(lhs)) {
    const auto &dyn_lhs = std::get<DynVal>(lhs);
    if (std::holds_alternative<DynVal>(rhs)) {
      // Both are DynVal
      return dyn_lhs * std::get<DynVal>(rhs);
    } else {
      // lhs is DynVal, rhs is int64_t
      return dyn_lhs * std::get<int64_t>(rhs);
    }
  } else {
    // lhs is int64_t, rhs is DynVal
    return std::get<int64_t>(lhs) * std::get<DynVal>(rhs);
  }
}

// This struct is used to store the tensor related information, which is
// extracted from numpy/torch tensor by __dlpack__ protocol.
struct TensorDescriptor {
  void *ptr;
  int64_t *shape;
  int64_t *stride;
  ShapeElement *shape_all;
  ShapeElement *stride_all;
  int8_t dtype_code;
  int8_t dtype_bits;
  int8_t dtype_lanes;
  DLDeviceType device_type;
  int32_t device_id;
  int32_t ndim;
  int32_t element_size_bytes;
  int32_t assumed_align;
  bool is_in_device;
  TensorFormat format;
  MemRefDescriptor *memref;
  size_t memref_size_in_bytes;
};

// 1. override_dtype is not used yet. Remember to remove it if not
// used finally.
// 2. if we transfer enum parameter with default value from Python to C++, it
// will trigger a import std::cast error. So, we use int to transfer enum
// parameter.
nb::object dlpack_to_tensor_desc(nb::object tensor, int format_int = 0,
                                 nb::object assumed_align = nb::none(),
                                 nb::object override_dtype = nb::none()) {
  // check if __dlpack__ attribute exists
  if (!nb::hasattr(tensor, "__dlpack__")) {
    throw std::runtime_error(
        "Input tensor does not support __dlpack__ protocol");
  }

  nb::object dlpack_method = tensor.attr("__dlpack__");
  nb::object dlpack_data = nb::none();

  try {
    // Note: torch cpu/cuda tensor and numpy array all call this non argument
    // version. If we call argument version first, and call non argument version
    // in the catch block, torch cuda tensor input will bring 15us cost (which
    // may be caused by the synchronization in dlpack_method).
    dlpack_data = dlpack_method();
  } catch (const std::exception &) {
    try {
      dlpack_data = dlpack_method(0);
    } catch (const std::exception &e) {
      throw std::runtime_error(std::string("Failed to call __dlpack__: ") +
                               e.what());
    }
  }

  if (!PyObject_TypeCheck(dlpack_data.ptr(), &PyCapsule_Type)) {
    throw std::runtime_error("__dlpack__() did not return a valid capsule");
  }

  const char *name = PyCapsule_GetName(dlpack_data.ptr());
  void *ptr = PyCapsule_GetPointer(dlpack_data.ptr(), name);
  if (!ptr) {
    throw std::runtime_error("Capsule data is null");
  }

  auto *dlpack_tensor = static_cast<DLManagedTensor *>(ptr);
  DLTensor *dl_tensor = &dlpack_tensor->dl_tensor;

  void *data_ptr = dl_tensor->data;
  DLDeviceType device_type = dl_tensor->device.device_type;
  int32_t device_id = dl_tensor->device.device_id;
  int32_t ndim = dl_tensor->ndim;
  int64_t *shape = dl_tensor->shape;
  int64_t *strides = dl_tensor->strides;
  int8_t dtype_code = dl_tensor->dtype.code;
  int8_t dtype_bits = dl_tensor->dtype.bits;
  int8_t dtype_lanes = dl_tensor->dtype.lanes;
  uint64_t byte_offset = dl_tensor->byte_offset;
  // Note: In class TensorDescriptor (python), element_size_bytes =
  // dl_tensor->dtype.bits / 8.
  int32_t element_size_bytes = (dtype_bits * dtype_lanes + 7) / 8;
  void *tensor_data_ptr =
      static_cast<void *>(static_cast<char *>(data_ptr) + byte_offset);

  if (dtype_lanes != 1) {
    throw std::runtime_error(
        "Unsupported lanes value: " + std::to_string(dtype_lanes) +
        ". Only scalar types (lanes=1) are supported.");
  }

  // Allocate memory for shape and stride arrays
  auto *shape_copy = new int64_t[ndim];
  int64_t *stride_copy = nullptr;
  auto *shape_all = new ShapeElement[ndim];
  auto *stride_all = new ShapeElement[ndim];

  memcpy(shape_copy, shape, ndim * sizeof(int64_t));
  for (int i = 0; i < ndim; i++) {
    shape_all[i] = shape[i];
  }

  // Copy stride data if available, otherwise compute it
  if (strides) {
    stride_copy = new int64_t[ndim];
    memcpy(stride_copy, strides, ndim * sizeof(int64_t));
  } else {
    // Default is row-major strides from dlpack doc
    // (https://github.com/dmlc/dlpack/blob/main/include/dlpack/dlpack.h#L259).
    stride_copy = new int64_t[ndim];
    int64_t acc = 1;
    for (int i = ndim - 1; i >= 0; i--) {
      stride_copy[i] = acc;
      acc *= shape[i];
    }
  }
  for (int i = 0; i < ndim; i++) {
    stride_all[i] = stride_copy[i];
  }

  bool is_in_device = false;
  switch (device_type) {
  case kDLCUDA:
    is_in_device = true;
    break;
  case kDLCPU:
  case kDLCUDAHost:
    is_in_device = false;
    break;
  default:
    throw std::runtime_error("Unsupported or unknown device type: " +
                             std::to_string(device_type));
  }

  int32_t new_assumed_align = -1;
  if (!assumed_align.is_none()) {
    new_assumed_align = nb::cast<int32_t>(assumed_align);
  } else {
    if (is_in_device) {
      new_assumed_align = 32;
    } else {
      new_assumed_align = element_size_bytes;
    }
  }
  // cute_ir does not check alignment, so we temporarily comment out this
  // assertion code.
  // if (reinterpret_cast<uintptr_t>(tensor_data_ptr) %
  // new_assumed_align != 0)
  // {
  //   throw std::runtime_error("Tensor data pointer is not aligned to " +
  //                            std::to_string(new_assumed_align));
  // }

  TensorFormat tensor_format = static_cast<TensorFormat>(format_int);

  // allocate memory for MemRefDescriptor with flexible array members
  size_t memref_size = sizeof(MemRefDescriptor);
  void *buffer = malloc(memref_size);
  if (!buffer) {
    throw std::runtime_error("Failed to allocate memory for MemRefDescriptor");
  }
  memset(buffer, 0, memref_size);

  auto *memref = static_cast<MemRefDescriptor *>(buffer);
  memref->ptr = tensor_data_ptr;

  TensorDescriptor *tensor_desc = nullptr;
  try {
    tensor_desc = new TensorDescriptor{.ptr = tensor_data_ptr,
                                       .shape = shape_copy,
                                       .stride = stride_copy,
                                       .shape_all = shape_all,
                                       .stride_all = stride_all,
                                       .dtype_code = dtype_code,
                                       .dtype_bits = dtype_bits,
                                       .dtype_lanes = dtype_lanes,
                                       .device_type = device_type,
                                       .device_id = device_id,
                                       .ndim = ndim,
                                       .element_size_bytes = element_size_bytes,
                                       .assumed_align = new_assumed_align,
                                       .is_in_device = is_in_device,
                                       .format = tensor_format,
                                       .memref = memref,
                                       .memref_size_in_bytes = memref_size};
  } catch (const std::exception &e) {
    delete[] shape_copy;
    delete[] stride_copy;
    delete[] shape_all;
    delete[] stride_all;
    free(buffer);
    throw std::runtime_error(
        std::string("Failed to allocate memory for TensorDescriptor: ") +
        e.what());
  }

  // create capsule and return
  return nb::steal(PyCapsule_New(
      static_cast<void *>(tensor_desc), "tensor_descriptor",
      [](PyObject *capsule) {
        if (void *ptr = PyCapsule_GetPointer(capsule, "tensor_descriptor")) {
          TensorDescriptor *tensor_desc = static_cast<TensorDescriptor *>(ptr);
          delete[] tensor_desc->shape;
          delete[] tensor_desc->stride;
          if (tensor_desc->shape_all) {
            delete[] tensor_desc->shape_all;
          }
          if (tensor_desc->stride_all) {
            delete[] tensor_desc->stride_all;
          }
          free(tensor_desc->memref);
          delete tensor_desc;
        }
      }));
}

TensorDescriptor *get_tensor_descriptor(PyObject *capsule) {
  void *ptr = PyCapsule_GetPointer(capsule, "tensor_descriptor");
  if (!ptr) {
    throw std::runtime_error("Invalid tensor descriptor capsule");
  }
  return static_cast<TensorDescriptor *>(ptr);
}

std::vector<int64_t> compute_stride_order(TensorDescriptor *tensor_desc) {
  struct IndexShapeStride {
    int index;
    int64_t shape;
    int64_t stride;

    IndexShapeStride(int i, int64_t s, int64_t st)
        : index(i), shape(s), stride(st) {}
  };

  std::vector<IndexShapeStride> index_shape_stride;
  index_shape_stride.reserve(tensor_desc->ndim);

  for (int i = 0; i < tensor_desc->ndim; ++i) {
    index_shape_stride.push_back(
        IndexShapeStride(i, tensor_desc->shape[i], tensor_desc->stride[i]));
  }

  // sort by stride in ascending order, then by shape in descending
  std::sort(index_shape_stride.begin(), index_shape_stride.end(),
            [](const IndexShapeStride &a, const IndexShapeStride &b) {
              // if stride is different, sort by stride in ascending
              if (a.stride != b.stride) {
                return a.stride < b.stride;
              }
              // if stride is the same, sort by shape in descending
              return a.shape > b.shape;
            });
  std::vector<int64_t> stride_order;
  for (int i = 0; i < tensor_desc->ndim; ++i) {
    stride_order.push_back(index_shape_stride[i].index);
  }
  return stride_order;
}

void update_memref(TensorDescriptor *tensor_desc,
                   std::vector<int64_t> shape_dyn,
                   std::vector<int64_t> stride_dyn) {
  auto *memref = tensor_desc->memref;

  size_t shape_dim = shape_dyn.size();
  size_t stride_dim = stride_dyn.size();

  size_t new_memref_size =
      sizeof(MemRefDescriptor) + (shape_dim + stride_dim) * sizeof(int32_t);
  void *buffer = malloc(new_memref_size);
  if (!buffer) {
    throw std::runtime_error("Failed to allocate memory for MemRefDescriptor");
  }
  memset(buffer, 0, new_memref_size);

  auto *new_memref = static_cast<MemRefDescriptor *>(buffer);
  new_memref->ptr = memref->ptr;

  // fill shape values
  for (size_t i = 0; i < shape_dim; i++) {
    new_memref->shape_stride[i] = static_cast<int32_t>(shape_dyn[i]);
  }
  // fill stride values - need to calculate the correct offset
  int32_t *stride_ptr = new_memref->shape_stride + shape_dim;
  for (size_t i = 0; i < stride_dim; i++) {
    stride_ptr[i] = static_cast<int32_t>(stride_dyn[i]);
  }

  // free old memref and assign new memref
  if (memref) {
    free(memref);
  }
  tensor_desc->memref = new_memref;
  tensor_desc->memref_size_in_bytes = new_memref_size;
}

// Helper function to get value from int or DynVal
int64_t get_runtime_value(const ShapeElement &s) {
  if (std::holds_alternative<DynVal>(s)) {
    return std::get<DynVal>(s).value();
  }
  return std::get<int64_t>(s);
}

void mark_layout_dynamic_impl(nb::object capsule, nb::object mode = nb::none(),
                              int divisibility = 1) {
  auto *tensor_desc = get_tensor_descriptor(capsule.ptr());

  // compute shape_all
  std::vector<int64_t> shape_dyn;
  if (mode.is_none()) {
    // mark all dimensions to be dynamic
    for (int i = 0; i < tensor_desc->ndim; i++) {
      DynVal dyn_val(tensor_desc->shape[i], divisibility);
      tensor_desc->shape_all[i] = dyn_val;
    }
  } else {
    int32_t mode_int = nb::cast<int32_t>(mode);
    if (mode_int < tensor_desc->ndim) {
      DynVal dyn_val(get_runtime_value(tensor_desc->shape_all[mode_int]),
                     divisibility);
      tensor_desc->shape_all[mode_int] = dyn_val;
    } else {
      throw std::runtime_error("Index out of bounds when accessing shape_all");
    }
  }

  // compute shape_dyn
  for (int i = 0; i < tensor_desc->ndim; i++) {
    const auto &shape_element = tensor_desc->shape_all[i];
    if (std::holds_alternative<DynVal>(shape_element)) {
      shape_dyn.push_back(std::get<DynVal>(shape_element).value());
    }
  }

  // When tensor is compact, stride is inducted by product of shape,
  // else when tensor is non-compact, strides are assumed to be all dynamic
  // expects the lowest dimension if there is any shape marked dynamic.
  if (tensor_desc->format == TensorFormat::compact) {
    std::vector<int64_t> stride_order = compute_stride_order(tensor_desc);
    assert(stride_order.size() == static_cast<size_t>(tensor_desc->ndim));
    assert(tensor_desc->stride[stride_order[0]] == 1);

    // induction dynamic strides
    // Initialize all strides to 1
    std::vector<ShapeElement> new_strides(tensor_desc->ndim, 1);
    ShapeElement cur_stride = 1;

    for (int64_t idx : stride_order) {
      new_strides[idx] = cur_stride;
      cur_stride = cur_stride * tensor_desc->shape_all[idx];
    }
    for (int i = 0; i < tensor_desc->ndim; i++) {
      tensor_desc->stride_all[i] = new_strides[i];
    }
  } else {
    // Non-compact format
    // Check if any shape is dynamic
    bool has_dynamic_shape = false;
    for (int i = 0; i < tensor_desc->ndim; i++) {
      const auto &shape = tensor_desc->shape_all[i];
      if (std::holds_alternative<DynVal>(shape)) {
        has_dynamic_shape = true;
        break;
      }
    }

    if (has_dynamic_shape) {
      // Convert strides to dynamic values with divby = 1 except for stride == 1
      for (int32_t i = 0; i < tensor_desc->ndim; i++) {
        int64_t stride_val = tensor_desc->stride[i];
        if (stride_val != 1) {
          tensor_desc->stride_all[i] = DynVal(stride_val);
        } else {
          tensor_desc->stride_all[i] = stride_val; // 1
        }
      }
    } else {
      for (int32_t i = 0; i < tensor_desc->ndim; i++) {
        tensor_desc->stride_all[i] = tensor_desc->stride[i];
      }
    }
  }

  // compute stride_dyn
  std::vector<int64_t> stride_dyn;
  for (int i = 0; i < tensor_desc->ndim; i++) {
    const auto &stride_element = tensor_desc->stride_all[i];
    if (std::holds_alternative<DynVal>(stride_element)) {
      stride_dyn.push_back(std::get<DynVal>(stride_element).value());
    }
  }
  // update tensor_desc's memref's shape and stride
  update_memref(tensor_desc, shape_dyn, stride_dyn);
}

// Register the function with nanobind
NB_MODULE(dlpack_runtime, m) {
  // disable leak warnings which may be false positives.
  nb::set_leak_warnings(false);

  m.def("dlpack_to_tensor_desc", &dlpack_to_tensor_desc,
        "Convert from tensor object supporting __dlpack__() to a "
        "TensorDescriptor",
        nb::arg("tensor"), nb::arg("format") = 0,
        nb::arg("assumed_align") = nb::none(),
        nb::arg("override_dtype") = nb::none());

  m.def(
      "get_tensor_desc_ndim",
      [](nb::object capsule) {
        auto *desc = get_tensor_descriptor(capsule.ptr());
        return desc->ndim;
      },
      "Get the number of dimensions of the tensor descriptor");

  m.def(
      "get_tensor_desc_shape",
      [](nb::object capsule) {
        auto *desc = get_tensor_descriptor(capsule.ptr());
        int ndim = desc->ndim;
        PyObject *tuple = PyTuple_New(ndim);
        for (int i = 0; i < ndim; i++) {
          PyTuple_SET_ITEM(tuple, i, PyLong_FromLongLong(desc->shape[i]));
        }
        return nb::steal(tuple);
      },
      "Get shape from tensor descriptor");

  m.def(
      "get_tensor_desc_stride",
      [](nb::object capsule) {
        auto *desc = get_tensor_descriptor(capsule.ptr());
        int ndim = desc->ndim;
        PyObject *tuple = PyTuple_New(ndim);
        for (int i = 0; i < ndim; i++) {
          PyTuple_SET_ITEM(tuple, i, PyLong_FromLongLong(desc->stride[i]));
        }
        return nb::steal(tuple);
      },
      "Get stride from tensor descriptor");

  m.def(
      "get_tensor_desc_shape_all",
      [](nb::object capsule) {
        auto *desc = get_tensor_descriptor(capsule.ptr());
        if (desc->shape_all == nullptr) {
          throw std::runtime_error("shape_all is empty");
        }
        int ndim = desc->ndim;
        PyObject *tuple = PyTuple_New(ndim);
        for (int i = 0; i < ndim; i++) {
          const auto &shape_element = desc->shape_all[i];
          if (std::holds_alternative<int64_t>(shape_element)) {
            // For static shapes, create Python int
            PyTuple_SET_ITEM(tuple, i,
                             PyLong_FromLong(std::get<int64_t>(shape_element)));
          } else {
            // For dynamic shapes, create Python DynVal object
            const auto &dyn_val = std::get<DynVal>(shape_element);
            // PyObject *dyn_obj = nb::cast(dyn_val).release().ptr();
            nb::object obj = nb::cast(dyn_val);
            PyObject *dyn_obj = obj.ptr();
            Py_INCREF(dyn_obj);
            PyTuple_SET_ITEM(tuple, i, dyn_obj);
          }
        }
        return nb::steal(tuple);
      },
      "Get shape_all from tensor descriptor");

  m.def(
      "get_tensor_desc_stride_all",
      [](nb::object capsule) {
        auto *desc = get_tensor_descriptor(capsule.ptr());
        if (desc->stride_all == nullptr) {
          throw std::runtime_error("stride_all is empty");
        }
        int ndim = desc->ndim;
        PyObject *tuple = PyTuple_New(ndim);
        for (int i = 0; i < ndim; i++) {
          if (std::holds_alternative<int64_t>(desc->stride_all[i])) {
            PyTuple_SET_ITEM(
                tuple, i,
                PyLong_FromLongLong(std::get<int64_t>(desc->stride_all[i])));
          } else {
            auto dyn_val = std::get<DynVal>(desc->stride_all[i]);
            // PyObject *dyn_obj = nb::cast(dyn_val).release().ptr();
            nb::object obj = nb::cast(dyn_val);
            PyObject *dyn_obj = obj.ptr();
            Py_INCREF(dyn_obj);
            PyTuple_SET_ITEM(tuple, i, dyn_obj);
          }
        }
        return nb::steal(tuple);
      },
      "Get stride_all from tensor descriptor");

  m.def(
      "get_tensor_desc_data_ptr",
      [](nb::object capsule) {
        auto *desc = get_tensor_descriptor(capsule.ptr());
        return reinterpret_cast<uint64_t>(desc->ptr);
      },
      "Get data pointer from tensor descriptor");

  m.def(
      "get_tensor_desc_element_type",
      [](nb::object capsule) {
        auto *desc = get_tensor_descriptor(capsule.ptr());
        int dtype_code = desc->dtype_code;
        int dtype_bits = desc->dtype_bits;

        // return corresponding type (str) based on DLPack dtype_code
        switch (dtype_code) {
        case kDLInt:
          switch (dtype_bits) {
          case 8:
            return "Int8";
          case 16:
            return "Int16";
          case 32:
            return "Int32";
          case 64:
            return "Int64";
          default:
            throw std::runtime_error("Unsupported integer bit width: " +
                                     std::to_string(dtype_bits));
          }
        case kDLUInt:
          switch (dtype_bits) {
          case 8:
            return "UInt8";
          case 16:
            return "UInt16";
          case 32:
            return "UInt32";
          case 64:
            return "UInt64";
          default:
            throw std::runtime_error(
                "Unsupported unsigned integer bit width: " +
                std::to_string(dtype_bits));
          }
        case kDLFloat:
          switch (dtype_bits) {
          // TODO: need to check if this is correct for fp8
          // TODO: supports for other types, e.g., fp4
          case 8:
            return "Float8E5M2";
          case 16:
            return "Float16";
          case 32:
            return "Float32";
          case 64:
            return "Float64";
          default:
            throw std::runtime_error("Unsupported float bit width: " +
                                     std::to_string(dtype_bits));
          }
        case kDLBfloat:
          return "BFloat16";
        case kDLComplex:
          switch (dtype_bits) {
          case 64:
            return "Complex64";
          case 128:
            return "Complex128";
          default:
            throw std::runtime_error("Unsupported complex bit width: " +
                                     std::to_string(dtype_bits));
          }
        case kDLBool:
          return "Bool";
        default:
          throw std::runtime_error(
              "Unknown DLPack dtype code: " + std::to_string(dtype_code) +
              " and bit width: " + std::to_string(dtype_bits));
        }
      },
      "Get element type from tensor descriptor based on DLPack dtype_code "
      "and "
      "bits");

  m.def(
      "get_tensor_desc_dtype_code",
      [](nb::object capsule) {
        auto *desc = get_tensor_descriptor(capsule.ptr());
        return desc->dtype_code;
      },
      "Get DLPack dtype_code from tensor descriptor");

  m.def(
      "get_tensor_desc_dtype_bits",
      [](nb::object capsule) {
        auto *desc = get_tensor_descriptor(capsule.ptr());
        return desc->dtype_bits;
      },
      "Get DLPack dtype_bits from tensor descriptor");

  m.def(
      "get_tensor_desc_device_type",
      [](nb::object capsule) {
        auto *desc = get_tensor_descriptor(capsule.ptr());
        return static_cast<int32_t>(desc->device_type);
      },
      "Get device type from tensor descriptor");

  m.def(
      "get_tensor_desc_device_id",
      [](nb::object capsule) {
        auto *desc = get_tensor_descriptor(capsule.ptr());
        return desc->device_id;
      },
      "Get device id from tensor descriptor");

  m.def(
      "get_tensor_desc_is_in_device",
      [](nb::object capsule) {
        auto *desc = get_tensor_descriptor(capsule.ptr());
        return desc->is_in_device;
      },
      "Get if tensor descriptor is in device");

  m.def(
      "get_tensor_desc_element_size_in_bytes",
      [](nb::object capsule) {
        auto *desc = get_tensor_descriptor(capsule.ptr());
        return desc->element_size_bytes;
      },
      "Get element size in bytes from tensor descriptor");

  m.def(
      "get_tensor_desc_assumed_align",
      [](nb::object capsule) {
        auto *desc = get_tensor_descriptor(capsule.ptr());
        return desc->assumed_align;
      },
      "Get assumed alignment from tensor descriptor");

  m.def(
      "get_memref_c_pointer",
      [](nb::object capsule) {
        auto *desc = get_tensor_descriptor(capsule.ptr());
        return reinterpret_cast<uintptr_t>(desc->memref);
      },
      "Get c pointer from memref descriptor");

  m.def(
      "get_memref_size_in_bytes",
      [](nb::object capsule) {
        auto *desc = get_tensor_descriptor(capsule.ptr());
        return desc->memref_size_in_bytes;
      },
      "Get size in bytes of memref descriptor");

  m.def("mark_layout_dynamic_c", &mark_layout_dynamic_impl,
        "Mark layout dynamic for memref descriptor", nb::arg("capsule"),
        nb::arg("mode") = nb::none(), nb::arg("divisibility") = 1);

  // expose the TensorFormat enum to Python
  nb::enum_<TensorFormat>(m, "TensorFormat")
      .value("compact", TensorFormat::compact)
      .value("non_compact", TensorFormat::non_compact)
      .export_values();

  // Bind DynVal class
  nb::class_<DynVal>(m, "DynVal")
      // Constructor
      .def(nb::init<int64_t, int64_t>(),
           "Constructor with value and optional divisibility", nb::arg("value"),
           nb::arg("div") = 1)

      // Properties
      .def_prop_ro("value", &DynVal::value, "Get the value")
      .def_prop_ro("div", &DynVal::div, "Get the divisibility")

      // String representations
      .def("__str__", &DynVal::toString, nb::rv_policy::copy)
      .def("__repr__", &DynVal::toRepr, nb::rv_policy::copy);
}
