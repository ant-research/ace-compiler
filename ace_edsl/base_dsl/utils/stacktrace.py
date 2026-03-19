
"""
 This module provides stacktrace helper functions
"""

import os


def walk_to_top_module(start_path):
    """
    Walk up from the start_path to find the top-level Python module.

    :param start_path: The path to start from.
    :return: The path of the top-level module.
    """
    current_path = start_path

    while True:
        # Check if we are at the root directory
        if os.path.dirname(current_path) == current_path:
            break

        # Check for __init__.py
        init_file_path = os.path.join(current_path, "__init__.py")
        if os.path.isfile(init_file_path):
            # If __init__.py exists, move up one level
            current_path = os.path.dirname(current_path)
        else:
            # If no __init__.py, we are not in a module; stop
            break

    # If we reached the root without finding a module, return None
    if os.path.dirname(current_path) == current_path and not os.path.isfile(
        os.path.join(current_path, "__init__.py")
    ):
        return None

    # Return the path of the top-level module
    return current_path


def filter_stackframe(traceback, prefix_path):
    """
    Filter out stack frames from the traceback that belong to the specified module path.

    This function removes stack frames from the traceback whose file paths start with
    the given prefix_path, effectively hiding internal implementation details from
    the error traceback shown to users.

    :param traceback: The traceback object to filter.
    :param prefix_path: The path prefix to filter out from the traceback.
    :return: The filtered traceback with internal frames removed.
    """
    iter_prev = None
    iter_tb = traceback
    while iter_tb is not None:
        if os.path.abspath(iter_tb.tb_frame.f_code.co_filename).startswith(prefix_path):
            if iter_tb.tb_next:
                if iter_prev:
                    iter_prev.tb_next = iter_tb.tb_next
                else:
                    traceback = iter_tb.tb_next
        else:
            iter_prev = iter_tb
        iter_tb = iter_tb.tb_next
    return traceback


def filter_exception(value, module_dir):
    """
    Filter out internal implementation details from exception traceback.

    This function recursively processes an exception and its cause chain,
    removing stack frames that belong to the specified module directory.
    This helps to present cleaner error messages to users by hiding
    implementation details.

    :param value: The exception object to filter.
    :param module_dir: The module directory path to filter out from tracebacks.
    :return: The filtered exception with internal frames removed.
    """
    if hasattr(value, "__cause__") and value.__cause__:
        filter_exception(value.__cause__, module_dir)

    if hasattr(value, "__traceback__"):
        filter_stackframe(value.__traceback__, module_dir)
