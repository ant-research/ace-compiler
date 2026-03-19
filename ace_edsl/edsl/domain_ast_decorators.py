"""
AST Decorators for ace_edsl

These functions are registered with executor.set_functions() to handle
dynamic execution of loops and conditionals, generating AIR via operator overloading.

The AST preprocessor transforms:
- `for i in range_dynamic(...)` → `@loop_selector(...)` decorated function
- `if dynamic_expr(...)` → `@if_selector(...)` decorated function

These decorators then call executor methods, which call these functions
to execute the loop/if body and generate AIR operations.

Key insight: Unlike MLIR's scf.ForOp, ace_edsl uses AIR's loop
operations (new_loop_begin_range, new_loop_end). The loop body is executed
**once** to generate the IR pattern - the loop construct handles repetition.
"""

from typing import Callable, Optional, Any, List
from ..base_dsl.ast_helpers import executor
from ..base_dsl.utils.logger import log
from .core.air_value import AIRValue

# Global container reference for loop operations
# This is set by AceEDSL when generating AIR
_current_container = None
_current_func_scope = None
_loop_temp_counter = 0
_if_temp_counter = 0
_loop_accum_counter = 0

def set_current_container(container, func_scope=None):
    """Set the current container for loop operations."""
    global _current_container, _current_func_scope
    _current_container = container
    _current_func_scope = func_scope
    
def get_current_container():
    """Get the current container for loop operations."""
    return _current_container


def _next_loop_temp_name() -> str:
    """Generate a unique temporary name for loop-carried values."""
    global _loop_temp_counter
    name = f"__loop_tmp_{_loop_temp_counter}"
    _loop_temp_counter += 1
    return name

def _next_loop_accum_name() -> str:
    """Generate a unique temporary name for loop accumulator variables.

    These are used to emulate MLIR scf.ForOp's loop-carried values in AIR,
    which doesn't have native yield support. Each accumulator is initialized
    before the loop, read/written inside the body, and read after the loop.
    """
    global _loop_accum_counter
    name = f"__loop_accum_{_loop_accum_counter}"
    _loop_accum_counter += 1
    return name

def _next_if_temp_name() -> str:
    """Generate a unique temporary name for if-then-else results."""
    global _if_temp_counter
    name = f"__if_tmp_{_if_temp_counter}"
    _if_temp_counter += 1
    return name


def _is_dynamic_expression(value: Any) -> bool:
    """
    Check if a value is a dynamic expression (AIRValue).
    
    Args:
        value: Value to check
        
    Returns:
        True if value is AIRValue (dynamic), False if compile-time constant
    """
    return isinstance(value, AIRValue)


def _loop_execute_range_dynamic(
    func: Callable,
    start: Any,
    stop: Any,
    step: Any,
    used_args: List[Any],
    iter_args: List[Any],
    unroll: int = -1,
    unroll_full: bool = False,
) -> Any:
    """
    Execute a loop body function, generating AIR loop operations.
    
    AIR does not support loop-carried SSA values directly (no yield), so we store
    loop results using `new_stid` after the loop body and return a load node from
    the outer scope. This avoids returning nodes created inside loop bodies, which
    can trigger container assertion failures at `new_retv`.
    
    NOTE: MLIR's scf.ForOp supports loop-carried values via yields.
    AIR's do_loop requires explicit storage to carry values across loop iterations.
    
    Args:
        func: Loop body function (created by AST preprocessor)
        start: Loop start value (may be AIRValue or int)
        stop: Loop stop value (may be AIRValue or int)
        step: Loop step value (may be AIRValue or int)
        used_args: Arguments used in loop body
        iter_args: Arguments that are iterated/modified
        unroll: Unroll factor (-1 = no unroll)
        unroll_full: Whether to fully unroll
        
    Returns:
        Result of loop execution (iter_args after loop)
    """
    log().info("Executing dynamic loop: start=%s, stop=%s, step=%s", start, stop, step)
    
    container = get_current_container()
    if container is None:
        log().warning("No AIR container available, falling back to Python loop")
        return _loop_execute_python_fallback(func, start, stop, step, used_args, iter_args)
    
    # Only support constant bounds for now
    if _is_dynamic_expression(start) or _is_dynamic_expression(stop) or _is_dynamic_expression(step):
        raise NotImplementedError(
            "Dynamic loop bounds (AIRValue) not yet fully implemented. "
            "Loop bounds must be compile-time constants for now."
        )
    
    start_val = int(start)
    stop_val = int(stop)
    step_val = int(step)
    if step_val != 1:
        # AIR loop builder only supports range(start, end) with step 1
        log().warning("Non-unit step not supported by AIR loop builder; falling back to Python loop")
        return _loop_execute_python_fallback(func, start, stop, step, used_args, iter_args)
    
    # 1. Store iter_args into named accumulator variables BEFORE the loop.
    #    This emulates MLIR scf.ForOp's init_values: each loop-carried variable
    #    gets a named storage location that the loop body reads from and writes to.
    #    Without this, the loop body would always read the original value (e.g., p0)
    #    instead of the previous iteration's result.
    accum_names = []
    accum_air_values = []
    for arg in iter_args:
        if isinstance(arg, AIRValue):
            accum_name = _next_loop_accum_name()
            accum_names.append(accum_name)
            # Initialize: accum = initial_value (emulates scf.ForOp init_values)
            container.new_stid(accum_name, arg.value)
            # Create AIRValue that loads from this variable on each .value access
            # (emulates scf.ForOp block_args — fresh value each iteration)
            accum_air_values.append(AIRValue(
                node=None,
                container=container,
                shape=arg.shape,
                domain=arg.domain,
                temp_name=accum_name,
            ))
        else:
            accum_names.append(None)
            accum_air_values.append(arg)

    # 2. Begin loop (pushes loop body block)
    loop_node = container.new_loop_begin_range(start_val, stop_val)
    loop_index = container.new_loop_index(loop_node)
    index_value = AIRValue(loop_index, container)
    
    # 3. Execute loop body ONCE to generate IR pattern.
    #    Pass accumulator AIRValues (not original iter_args) so the body
    #    reads from the accumulator variables instead of the original values.
    loop_results = func(index_value, *used_args, *accum_air_values)
    
    container.new_loop_end()

    # 4. Store loop body results back to accumulator variables (still inside
    #    the loop body block, before new_loop_end). This creates the
    #    loop-carried dependency (emulates scf.YieldOp): each iteration
    #    reads the accumulator, computes, and writes the result back.
    for accum_name, res in zip(accum_names, loop_results):
        if accum_name is not None and isinstance(res, AIRValue):
            container.new_stid(accum_name, res.value)

    # 5. End loop (pop body block, emit do_loop)
    carried_results = []
    for accum_name, res in zip(accum_names, loop_results):
        if accum_name is not None:
            temp_name = _next_loop_temp_name()
            load_node = container.new_ldid(accum_name)
            carried_node = container.new_stid(temp_name, load_node)
            shape = res.shape if isinstance(res, AIRValue) else None
            carried_results.append(AIRValue(carried_node, container, shape))
        else:
            carried_results.append(res)
    
    return executor.converge_ret_val(carried_results)


def _loop_execute_python_fallback(
    func: Callable,
    start: Any,
    stop: Any,
    step: Any,
    used_args: List[Any],
    iter_args: List[Any],
) -> Any:
    """
    Fallback to Python loop execution when no AIR container is available.
    This is used for testing or when preprocessing isn't fully set up.
    """
    start_val = int(start) if not _is_dynamic_expression(start) else 0
    stop_val = int(stop) if not _is_dynamic_expression(stop) else 1
    step_val = int(step) if not _is_dynamic_expression(step) else 1
    
    loop_results = iter_args.copy() if iter_args else []
    
    for i in range(start_val, stop_val, step_val):
        loop_results = func(i, *used_args, *loop_results)
        
        if loop_results is None:
            loop_results = []
        if not isinstance(loop_results, list):
            loop_results = [loop_results]
    
    return executor.converge_ret_val(loop_results)


def _if_execute_dynamic(
    pred: Any,
    then_block: Callable,
    else_block: Optional[Callable] = None,
    used_args: List[Any] = None,
    yield_args: List[Any] = None,
) -> Any:
    """
    Execute if/else blocks dynamically, generating AIR operations.
    
    This is called when the predicate is dynamic (AIRValue).
    The then/else blocks are executed, and operator overloading generates AIR operations.
    
    Args:
        pred: Predicate value (may be AIRValue or bool)
        then_block: Then block function
        else_block: Else block function (optional)
        used_args: Arguments used in blocks
        yield_args: Arguments that are yielded/modified
        
    Returns:
        Result of executing then or else block
    """
    log().info("Executing dynamic if: pred=%s", pred)
    
    used_args = used_args or []
    yield_args = yield_args or []
    
    # Check if predicate is compile-time constant
    if not _is_dynamic_expression(pred):
        # Compile-time constant - execute Python if
        if pred:
            log().debug("Running then block")
            res = then_block(*used_args, *yield_args)
            return executor.converge_ret_val(res)
        elif else_block is not None:
            log().debug("Running else block")
            res = else_block(*used_args, *yield_args)
            return executor.converge_ret_val(res)
        return None
    
    # Dynamic predicate - generate AIR conditional operations
    container = get_current_container()
    if container is None:
        raise RuntimeError("No AIR container available for dynamic if lowering")

    if not hasattr(container, "new_if_begin"):
        raise RuntimeError("AIR container does not support if lowering")

    def _to_node(value: Any):
        if isinstance(value, AIRValue):
            return value.value
        if isinstance(value, int):
            return container.new_intconst(value)
        if isinstance(value, float):
            return container.new_intconst(int(value))
        return value

    def _normalize_results(result: Any) -> List[Any]:
        if result is None:
            return []
        if isinstance(result, list):
            return result
        return [result]

    # If there are no yield args, just emit the control flow and return
    if not yield_args:
        pred_node = pred.value if isinstance(pred, AIRValue) else pred
        container.new_if_begin(pred_node)
        then_block(*used_args)
        if else_block is not None:
            container.new_else()
            else_block(*used_args)
        container.new_if_end()
        return None

    # Create temporaries for merge results and seed with current values
    temp_names = []
    for arg in yield_args:
        temp_name = _next_if_temp_name()
        temp_names.append(temp_name)
        node = _to_node(arg)
        if node is not None:
            container.new_stid(temp_name, node)

    # Emit if/else blocks
    pred_node = pred.value if isinstance(pred, AIRValue) else pred
    container.new_if_begin(pred_node)
    then_results = _normalize_results(then_block(*used_args, *yield_args))
    for idx, res in enumerate(then_results):
        if idx >= len(temp_names):
            break
        node = _to_node(res)
        if node is not None:
            container.new_stid(temp_names[idx], node)

    if else_block is not None:
        container.new_else()
        else_results = _normalize_results(else_block(*used_args, *yield_args))
        for idx, res in enumerate(else_results):
            if idx >= len(temp_names):
                break
            node = _to_node(res)
            if node is not None:
                container.new_stid(temp_names[idx], node)

    container.new_if_end()

    if not hasattr(container, "new_ldid"):
        raise RuntimeError("Container missing new_ldid (rebuild bindings)")

    merged_results = [
        AIRValue(container.new_ldid(name), container)
        for name in temp_names
    ]

    return executor.converge_ret_val(merged_results)


def _while_execute_dynamic(
    pred: Any,
    while_before_block: Callable,
    while_after_block: Optional[Callable] = None,
    used_args: List[Any] = None,
    yield_args: List[Any] = None,
) -> Any:
    """
    Execute while loop dynamically, generating AIR operations.
    
    This is called when the predicate is dynamic (AIRValue).
    The while loop body is executed, and operator overloading generates AIR operations.
    
    Args:
        pred: Predicate value (may be AIRValue or bool)
        while_before_block: While loop body function
        while_after_block: After-block function (optional)
        used_args: Arguments used in loop
        yield_args: Arguments that are yielded/modified
        
    Returns:
        Result of executing while loop
    """
    log().info("Executing dynamic while: pred=%s", pred)
    
    used_args = used_args or []
    yield_args = yield_args or []
    
    # Check if predicate is compile-time constant
    if not _is_dynamic_expression(pred):
        # Compile-time constant - execute Python while
        loop_results = yield_args.copy() if yield_args else []
        
        while pred:
            loop_results = while_before_block(*used_args, *loop_results)
            if loop_results is None:
                loop_results = []
            if not isinstance(loop_results, list):
                loop_results = [loop_results]
            
            # Update predicate (would need to be passed back from block)
            # For now, this is a simplified version
            break
        
        if while_after_block is not None:
            loop_results = while_after_block(*used_args, *loop_results)
        
        return executor.converge_ret_val(loop_results)
    
    # Dynamic predicate - generate AIR while loop operations
    # TODO: Generate AIR while loop operations when predicate is dynamic
    raise NotImplementedError(
        "Dynamic while predicate (AIRValue) not yet fully implemented. "
        "Predicate must be compile-time constant for now."
    )
