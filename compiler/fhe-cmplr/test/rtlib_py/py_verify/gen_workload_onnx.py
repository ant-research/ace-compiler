"""Self-contained ONNX workload generators for Phase 1 e2e verification.

The standard onnx test set lives behind an oss download
(`oss://antsys-fhe/cti/onnx/op/`, unavailable in many environments -- the
existing `test_fhe_cmplr.sh` fails here on `ossutil64: command not found`).
These generators build the small workloads the Python-backend execution gate
needs directly with the `onnx` package (1.22.0, present in the env), so the
e2e driver depends on no oss and no onnxruntime.

Each builder writes an opset-13 model with a low `ir_version` (7) for broad
compatibility with the C++ onnx loader in the fhe_cmplr frontend. The shapes:

  add1   A[n] + B[n]  -> Y[n]      (ciph + ciph -> add_ciph)
  mul1   A[n] * B[n]  -> Y[n]      (ciph * ciph -> mul_ciph -> rescale -> relin)
  mulc2  A[n] * c     -> Y[n]      (constant-folded -> encode_float(_cst_0) +
                                    mul_plain + rescale; the scalar `c` is a
                                    raw_data float32 initializer the frontend
                                    lowers to the `_cst_0` ARRAY constant; Phase
                                    2.2 emits `_cst_0 = [c]` so the backend runs)

n defaults to 1 (scalar, the shape the Phase 1.0-1.4 emit-gate already
proved compiles). add8 passes n=8 to exercise slot-vector elementwise add.
"""

import numpy as np
import onnx
from onnx import helper, numpy_helper, TensorProto

OPSET = 13
IR_VERSION = 7  # low, for loader compatibility


def _save(model, path):
    model.ir_version = IR_VERSION
    onnx.checker.check_model(model)
    onnx.save(model, path)


def _value_info(name, n):
    shape = [n] if n is not None else []
    return helper.make_tensor_value_info(name, TensorProto.FLOAT, shape)


def build_add(path, n=1):
    A, B, Y = _value_info("A", n), _value_info("B", n), _value_info("Y", n)
    add = helper.make_node("Add", ["A", "B"], ["Y"])
    graph = helper.make_graph([add], "add_graph", [A, B], [Y])
    _save(helper.make_model(graph, opset_imports=[helper.make_opsetid("", OPSET)]),
          path)


def build_mul(path, n=1):
    A, B, Y = _value_info("A", n), _value_info("B", n), _value_info("Y", n)
    mul = helper.make_node("Mul", ["A", "B"], ["Y"])
    graph = helper.make_graph([mul], "mul_graph", [A, B], [Y])
    _save(helper.make_model(graph, opset_imports=[helper.make_opsetid("", OPSET)]),
          path)


def build_mul_const(path, n=1, c=2.5):
    # A * c, with c a constant initializer (the frontend constant-folds the
    # scalar -> encode_float(_cst_0) + mul_plain + rescale). The constant is
    # serialized as `raw_data` (little-endian float32 bytes) via
    # numpy_helper.from_array -- the form real ONNX models and the oss test set
    # use, and the only form the fhe_cmplr frontend lowerer (air_const.cxx)
    # currently loads. A float_data-only tensor (helper.make_tensor with a
    # Python list) instead hits the frontend's raw-format assert
    # (air_const.cxx:93) before the Python backend runs, so it could never
    # exercise the Phase 2.2 array-constant emitter.
    A = _value_info("A", n)
    Y = _value_info("Y", n)
    cst = numpy_helper.from_array(np.array([float(c)], dtype=np.float32),
                                 name="cst")
    mul = helper.make_node("Mul", ["A", "cst"], ["Y"])
    graph = helper.make_graph([mul], "mulc_graph", [A], [Y], initializer=[cst])
    _save(helper.make_model(graph, opset_imports=[helper.make_opsetid("", OPSET)]),
          path)


def build_mul_mul(path, n=1):
    # A * B * C -- two chained Mul nodes (depth 2: mul_ciph -> rescale -> relin
    # twice). The rtlib-py-onnx plan P4a deep-context probe (gap #1/#2: a
    # depth-2 chain needs towersLeft >= nsd under rt_openfhe; compile with a
    # raised context -CKKS:icl=3 -CKKS:mcl=5 -CKKS:sf=40). rt_ref is float64
    # plaintext-exact -> max_abs=0 for any inputs; rt_openfhe is green within
    # the section E depth-2 tolerance or fails with a [hard] droplevel (recorded as
    # fail-and-investigate, not a regression).
    A, B, C = _value_info("A", n), _value_info("B", n), _value_info("C", n)
    Y = _value_info("Y", n)
    t = "t_ab"  # intermediate A*B (inferred; no graph value_info needed)
    mul1 = helper.make_node("Mul", ["A", "B"], [t])
    mul2 = helper.make_node("Mul", [t, "C"], ["Y"])
    graph = helper.make_graph([mul1, mul2], "mulmul_graph", [A, B, C], [Y])
    _save(helper.make_model(graph, opset_imports=[helper.make_opsetid("", OPSET)]),
          path)


def build_concat(path, n=2):
    # Single Concat node over two 4-D (NCHW) inputs on axis=1 (channel) ->
    # Y[1, 2n, 1, 1]. The frontend's Concat lowering (nn-addon air_stmt.cxx:969)
    # requires 4-D inputs (the corpus squeezenet Concats are feature maps), so a
    # 1-D synthetic model is rejected at compile -- this 4-D form matches the
    # real lowering path. The rtlib-py-onnx plan P4d probe: the onnx-models
    # corpus has NO isolated Concat model (all 8 Concat nodes live inside
    # alinm/squeezenet), so this synthetic single-node model takes the
    # isolated-Concat bucket attribution.
    A = helper.make_tensor_value_info("A", TensorProto.FLOAT, [1, n, 1, 1])
    B = helper.make_tensor_value_info("B", TensorProto.FLOAT, [1, n, 1, 1])
    Y = helper.make_tensor_value_info("Y", TensorProto.FLOAT, [1, 2 * n, 1, 1])
    cat = helper.make_node("Concat", ["A", "B"], ["Y"], axis=1)
    graph = helper.make_graph([cat], "concat_graph", [A, B], [Y])
    _save(helper.make_model(graph, opset_imports=[helper.make_opsetid("", OPSET)]),
          path)


if __name__ == "__main__":
    import sys
    # `python3 gen_workload_onnx.py <name> <path> [n]`  (name in add/mul/mulc/
    # mulmul/concat). mulmul takes 3 inputs (A,B,C); concat takes 2 (A,B) and
    # emits a 2n output.
    name, path = sys.argv[1], sys.argv[2]
    n = int(sys.argv[3]) if len(sys.argv) > 3 else 1
    {"add": build_add, "mul": build_mul, "mulc": build_mul_const,
     "mulmul": build_mul_mul, "concat": build_concat}[name](path, n)
    print(f"wrote {name} onnx -> {path}")