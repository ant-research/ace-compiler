"""
This module provides jit executor related classes
"""
import io
import inspect
import ctypes
import numpy as np
from typing import get_origin

from .utils.timer import timer
from .utils.logger import log
from .common import DSLRuntimeError

from .runtime import Argument
from .runtime.tensor_descriptor import TensorDescriptor
#from .runtime import cuda as cuda_helpers
# dlpack removed - ace_edsl uses AIR, not MLIR/GPU runtime
def dlpack_to_tensor_desc(*args, **kwargs):
    raise NotImplementedError("dlpack_runtime removed - ace_edsl uses AIR, not MLIR/GPU runtime")

from . import typing as t

# MLIR imports removed - ace_edsl uses AIR instead
ir = None


class CudaSingleModule:
    def __init__(self, cuda_module, kernel_ptr):
        self.cuda_module = cuda_module
        self.kernel_ptr = kernel_ptr


class CudaModules:
    def __init__(self, modules, args):
        # list of CudaSingleModule
        self.modules = modules
        # extra kernel ptr arguments for launch
        self.args = args


class JitExecutor:
    def __init__(
        self,
        dsl,
        engine,
        capi_func,
        ir_module,
        args_spec,
        cuda_modules: CudaModules = None,
        jit_time_profiling=False,
    ):
        # TODO: remove dsl depedency of numpy_to_memref
        self.dsl = dsl
        self.engine = engine
        self.capi_func = capi_func
        self.ir_module = ir_module
        self.args_spec = args_spec
        if args_spec is not None:
            self.args_spec = self.filter_runtime_arg_spec(args_spec)
        # cuda kernels
        self.cuda_modules = cuda_modules
        self.jit_time_profiling = jit_time_profiling
        self._tensor_capsules = []

    def filter_runtime_arg_spec(self, arg_spec: inspect.FullArgSpec):
        runtime_args = []
        runtime_annotations = {}
        runtime_defaults = []

        # Calculate the offset where defaults start in the original args
        if arg_spec.defaults:
            defaults_start_idx = len(arg_spec.args) - len(arg_spec.defaults)
        else:
            defaults_start_idx = len(arg_spec.args)

        # Filter arguments and maintain their properties
        for i, arg_name in enumerate(arg_spec.args):
            arg_type = arg_spec.annotations.get(arg_name, None)

            # Skip compile-time arguments
            if i == 0 and arg_name == "self":
                continue
            if isinstance(arg_type, t.IRConst):
                continue
            elif (isinstance(arg_type, type) and issubclass(arg_type, t.Constexpr)) or (
                get_origin(arg_type) is t.Constexpr
            ):
                continue
            # Keep runtime arguments
            runtime_args.append(arg_name)
            if arg_name in arg_spec.annotations:
                runtime_annotations[arg_name] = arg_type

            # Keep corresponding default if it exists
            if i >= defaults_start_idx:
                default_idx = i - defaults_start_idx
                runtime_defaults.append(arg_spec.defaults[default_idx])

        # Filter kwonlyargs and their defaults
        runtime_kwonlyargs = []
        runtime_kwonlydefaults = {}

        if arg_spec.kwonlyargs:
            for kwarg in arg_spec.kwonlyargs:
                arg_type = arg_spec.annotations.get(kwarg, None)

                # Apply same filtering logic
                if isinstance(arg_type, t.IRConst):
                    continue
                elif (
                    isinstance(arg_type, type) and issubclass(arg_type, t.Constexpr)
                ) or (get_origin(arg_type) is t.Constexpr):
                    continue

                runtime_kwonlyargs.append(kwarg)
                if kwarg in arg_spec.annotations:
                    runtime_annotations[kwarg] = arg_type
                if arg_spec.kwonlydefaults and kwarg in arg_spec.kwonlydefaults:
                    runtime_kwonlydefaults[kwarg] = arg_spec.kwonlydefaults[kwarg]

        # Convert runtime_defaults to tuple if not empty (as expected by FullArgSpec)
        runtime_defaults = tuple(runtime_defaults) if runtime_defaults else None

        return inspect.FullArgSpec(
            args=runtime_args,
            varargs=arg_spec.varargs,  # Keep original varargs
            varkw=arg_spec.varkw,  # Keep original varkw
            defaults=runtime_defaults,
            kwonlyargs=runtime_kwonlyargs,
            kwonlydefaults=runtime_kwonlydefaults if runtime_kwonlydefaults else None,
            annotations=runtime_annotations,
        )

    def __del__(self):
        if self.cuda_modules:
            cuda_modules = [module.cuda_module for module in self.cuda_modules.modules]
            for module in set(cuda_modules):
                cuda_helpers.unload_cubin_module(module)

    def generate_execution_args(self, input_args, arg_spec: inspect.FullArgSpec):
        """
        This function is the prune version of `generate_mlir_function_types` which only generates execution args
        to get rid of mlir context.
        """
        if len(input_args) != len(arg_spec.args):
            raise DSLRuntimeError(
                f"Input args length {len(input_args)} != runtime arg_spec.args length {len(arg_spec.args)}"
            )

        exe_args = []
        self._tensor_capsules = []
        for i, arg in enumerate(input_args):
            arg_type = arg_spec.annotations.get(arg_spec.args[i], None)
            if type(arg_type) is type and issubclass(arg_type, t.IRVariadic):
                exe_args.extend(arg.operands)

            elif isinstance(arg, Argument):
                exe_args.append(arg.c_pointer())

            elif type(arg_type) is type and self.dsl._is_tensor_descriptor(arg):
                if isinstance(arg, TensorDescriptor):
                    memref_arg = (
                        self.dsl._generate_executable_arg_for_tensor_descriptor(
                            tensor=arg
                        )
                    )
                else:
                    # dlpack removed - cannot transform tensor
                    raise NotImplementedError("dlpack removed - cannot transform tensor to dlpack format")
                    memref_arg = self.dsl._generate_executable_arg_for_tensor_capsule(
                        tensor_capsule
                    )
                    self._tensor_capsules.append(tensor_capsule)
                exe_args.append(memref_arg)
                continue

            elif isinstance(arg, np.ndarray):
                memref_arg, _ = self.dsl.numpy_to_memref(arg_type, arg)
                exe_args.append(memref_arg)

            else:
                # If not any known type annotation, assume it's a dynamic value
                # and try to get the MLIR type/value

                # Implicit cast to NumericMeta
                if isinstance(arg_type, t.NumericMeta):
                    arg = t.cast(arg, arg_type)

                if hasattr(arg, "__c_pointers__"):
                    # TODO: This is just a temporary WAR before fully adopting enhanced DslType
                    # Should be replaced with
                    #   >>> if hasattr(arg, "__c_pointers__"):
                    #   >>>     types.extend(type(arg).__mlir_types__())
                    #   >>>     exe_args.extend(arg.__c_pointers__())
                    #
                    # In order to reduce overhead of calling JIT function, we can define `unsafe_call`
                    #  which assuming user provide arg in C-style, e.g.
                    #
                    #   >>> @jit
                    #   >>> def func(a: Int32, ...):
                    #   >>>     ...
                    #   >>>
                    #   >>> a = ...
                    #   >>>
                    #   >>> unsafe_call(func, ctypes.c_void_p(ctypes.c_int32(a)), ...)
                    #
                    exe_args.extend(arg.__c_pointers__())

        return exe_args

    def __call__(self, *args):
        exe_args = self.generate_execution_args(args, self.args_spec)

        self.run_compiled_program(exe_args)

    # Assume each execution args has type `c_void_p` to reduce the overhead of `ctypes.cast`.
    def get_invoke_packed_args(self, exe_args):
        if self.cuda_modules:
            exe_args += self.cuda_modules.args
        packed_args = (ctypes.c_void_p * len(exe_args))()
        for argNum in range(len(exe_args)):
            packed_args[argNum] = exe_args[argNum]
        return packed_args

    def run_compiled_program(self, exe_args):
        if self.jit_time_profiling:
            profiler = timer(enable=True)
            try:
                packed_args = profiler(self.get_invoke_packed_args)(exe_args)
                profiler(self.capi_func)(packed_args)
            except Exception as e:
                raise DSLRuntimeError(f"💥💥💥 Runtime Crash 💥💥💥", cause=e)
        else:
            try:
                packed_args = self.get_invoke_packed_args(exe_args)
                self.capi_func(packed_args)
            except Exception as e:
                raise DSLRuntimeError(f"💥💥💥 Runtime Crash 💥💥💥", cause=e)

    def update_jit_cuda_modules(self, kernel_symbols):
        # preload cuda module from compiled cubin in ir and store to jit_executor.kernels.
        if len(kernel_symbols) > 0:
            extra_args = []
            module = self.ir_module
            cuda_kernel_cache = dict()
            for sym in kernel_symbols:
                if sym not in cuda_kernel_cache:
                    log().debug(f"Loading CUDA module for symbol: {sym}")

                    # load cuda module/get function pointer from module and cache
                    def walk_callback(sym, func_sym, cubin_data):
                        cubin_module = cuda_helpers.load_cubin_module_data(cubin_data)
                        kernel_ptr = cuda_helpers.get_kernel_function(
                            cubin_module, func_sym
                        )
                        cuda_kernel_cache[sym] = CudaSingleModule(
                            cubin_module, kernel_ptr
                        )

                    self.walk_module_and_get_cubin_data(module, sym, walk_callback)
                else:
                    log().debug(f"Symbol {sym} already in cache")
                # check if kernel is empty.
                if sym in cuda_kernel_cache:
                    extra_args.append(
                        ctypes.c_void_p(cuda_kernel_cache[sym].kernel_ptr.getPtr())
                    )
            # store to the jit result if jit result is cached.
            self.cuda_modules = CudaModules(cuda_kernel_cache.values(), extra_args)

        return self

    def _get_escaped_cubin_bytes(self, cubin_data):
        """This function escapes cubin data from mlir raw bytecode to executable binary bytes"""

        def ishex(inp):
            return (
                inp in range(0x30, 0x3A)
                or inp in range(0x61, 0x67)
                or inp in range(0x41, 0x47)
            )

        converted = bytearray()
        idx = 0
        while idx < len(cubin_data):
            # escape the original bytes
            if cubin_data[idx] == 0x5C:
                # if data of idx is b'\\'
                if ishex(cubin_data[idx + 1]) and ishex(cubin_data[idx + 2]):
                    converted += bytearray.fromhex(
                        cubin_data[idx + 1 : idx + 3].decode()
                    )
                    idx += 3
                elif cubin_data[idx + 1] == 0x5C:
                    converted.append(cubin_data[idx])
                    idx += 2
            else:
                # no escape, directly write
                converted.append(cubin_data[idx])
                idx += 1
        return bytes(converted)

    def walk_module_and_get_cubin_data(self, module, sym, callback):
        """This function is used to walk gpu binary op, extract the cubin inside, and process cubin data with callback."""

        def walk_gpu_binary_op(op):
            if op.name != "gpu.binary":
                return ir.WalkResult.ADVANCE
            s = io.BytesIO()
            op.write_bytecode(s)
            cubin_data = s.getvalue()
            if sym.encode() not in cubin_data:
                return ir.WalkResult.ADVANCE

            # mlir symbol of jit_gpu_inner(gpu.launch) is concrete, while mlir symbol of kernel(gpu.launch_func) is "kernels".
            if (
                "kernels" != op.opview.sym_name.value
                and sym != op.opview.sym_name.value
            ):
                return ir.WalkResult.ADVANCE
            # function symbol of kernel(gpu.launch_func) is equal to sym name in mlir
            func_sym = sym
            # function symbol of jit_gpu_inner(gpu.launch) is not equal to sym name in mlir, so we have to get the actual function symbol by removing mlir sym's postfix.
            if sym == op.opview.sym_name.value and not sym.endswith("_kernel"):
                func_sym = sym.rsplit("_", 1)[0]

            cubin_data = cubin_data.split(b'bin = "')[1].split(b'">')[0]
            cubin_data = self._get_escaped_cubin_bytes(cubin_data)
            callback(sym, func_sym, cubin_data)
            return ir.WalkResult.ADVANCE

        module.operation.walk(walk_gpu_binary_op)
