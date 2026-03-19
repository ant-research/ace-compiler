"""
This module provides utilities for the environment variables setup.

It provides an EnvironmentVarManager, which reads environment variables for the DSL
and caches them for efficient access.

It also provides utilities to automatically setup a subset of environment variables
based on heuristics.
"""

import os
import sys
import shutil
import glob
from pathlib import Path
from functools import lru_cache
from typing import Any

#from ..base_dsl.runtime.cuda  import get_compute_capability_major_minor
from .utils.logger import log

IS_WINDOWS = sys.platform == "win32"
CLIB_EXT = ".dll" if IS_WINDOWS else ".so"

# =============================================================================
# Environment Variable Helpers
# =============================================================================


@lru_cache(maxsize=None)
def get_str_env_var(var_name, default_value=None):
    value = os.getenv(var_name)
    return value if value is not None else default_value

@lru_cache(maxsize=None)
def get_bool_env_var(var_name, default_value=False):
    value = get_str_env_var(var_name)
    if value is None:
        return default_value
    return value not in {"False", "0", ""}


@lru_cache(maxsize=None)
def get_int_env_var(var_name, default_value=0):
    value = get_str_env_var(var_name)
    return int(value) if value and value.isdigit() else default_value


@lru_cache(maxsize=None)
def get_env_var(var_name: str, default_value: Any = None) -> Any:
    if isinstance(default_value, bool):
        return get_bool_env_var(var_name, default_value)
    elif isinstance(default_value, int):
        return get_int_env_var(var_name, default_value)
    else:
        return get_str_env_var(var_name, default_value)


def detect_gpu_arch(prefix):
    """
    Attempts to detect the machine's GPU architecture.

    Returns:
        A string representing the GPU architecture (e.g. "70" for compute capability 7.0),
        or a default value(e.g. "sm_100") if the GPU architecture cannot be determined.
    """
    arch = (None, None)
    #try:
    #    arch = get_compute_capability_major_minor()
    #except Exception as e:
    #    log().info(f"Failed to get CUDA compute capability: {e}")

    if arch == (None, None):
        # default to sm_100
        arch = (10, 0)

    major, minor = arch
    suffix = ""
    if major >= 9 and minor >= 0:
        if prefix.lower().startswith("cute"):
            suffix = "a"
    elif minor != 0:
        # e.g sm_86, belong with sm_80 family
        minor = 0
    return f"sm_{major}{minor}{suffix}"


def find_libs_in_ancestors(start, target_libs, lib_folder_guesses):
    """
    Search ancestor directories for a candidate library folder containing all required libraries.

    Starting from the given path, this function traverses up through each parent directory.
    For every ancestor, it checks candidate subdirectories (specified by lib_folder_guesses)
    for files that match the required library extension (CLIB_EXT). Library file names are
    canonicalized by removing the "lib" prefix from their stem. If a candidate directory contains
    all of the required libraries (as specified in target_libs), the function returns a list of
    absolute paths to these library files.

    Parameters:
        start (str or Path): The starting directory from which to begin the search.
        target_libs (iterable of str): A collection of required library names (without the "lib" prefix).
        lib_folder_guesses (iterable of str): Relative paths from an ancestor directory that may contain the libraries.

    Returns:
        list[str] or None: A list of resolved paths to the required library files if found; otherwise, None.
    """
    # Traverse through all parent directories of the resolved starting path.
    for ancestor in Path(start).resolve().parents:
        # Iterate over each candidate relative directory path.
        for rel_path in lib_folder_guesses:
            target_dir = ancestor / rel_path
            # Skip if the candidate directory does not exist.
            if not target_dir.is_dir():
                continue

            # Initialize a list to hold the resolved paths of matching library files.
            libs_cand = []
            # Create a set of the remaining libraries we need to find.
            remaining_libs = set(target_libs)

            # Iterate over all items in the candidate directory.
            for p in target_dir.iterdir():
                # Consider only files with the expected library extension.
                if p.suffix == CLIB_EXT:
                    # Canonicalize the library name by removing the "lib" prefix.
                    lib_name = p.stem.removeprefix("lib")
                    # If this library is required, add its resolved path and mark it as found.
                    if lib_name in remaining_libs:
                        libs_cand.append(str(p.resolve()))
                        remaining_libs.remove(lib_name)

            # If all required libraries have been found, return the list of library paths.
            if len(remaining_libs) == 0:
                return libs_cand

    # Return None if no candidate directory contains all required libraries.
    return None


def _find_cuda_home():
    """Find the CUDA installation path using a series of heuristic methods.
    Methods below are checked in order, and the function returns on first match:
    1. Checking the environment variables CUDA_HOME and CUDA_PATH.
    2. Searching for the 'nvcc' compiler in the system PATH and deriving the path of cuda.
    3. Scanning common installation directories based on the operating system.
       - On Windows systems (when IS_WINDOWS is True), it searches in:
             C:/Program Files/CUDA/v*.*
       - On Unix-like systems, it searches in:
             /usr/local/cuda*

    Returns:
        Optional[str]: The absolute CUDA installation path if found; otherwise, None.

    Note:
        The variable IS_WINDOWS is defined in the module scope.
    """
    # Guess #1
    cuda_home = get_env_var("CUDA_HOME") or get_env_var("CUDA_PATH")
    if cuda_home is None:
        # Guess #2
        nvcc_path = shutil.which("nvcc")
        if nvcc_path is not None:
            cuda_home = os.path.dirname(os.path.dirname(nvcc_path))
        else:
            # Guess #3
            if IS_WINDOWS:
                glob_pat = "C:/Program Files/CUDA/v*.*"
            else:
                glob_pat = "/usr/local/cuda*"
            cuda_homes = glob.glob(glob_pat)
            if len(cuda_homes) == 0:
                cuda_home = ""
            else:
                cuda_home = cuda_homes[0]
            if not os.path.exists(cuda_home):
                cuda_home = None
    return cuda_home


def get_cuda_toolkit_path():
    """
    Get cuda_toolkit_path. It returns get_env_var('CUDA_TOOLKIT_PATH') if set.
    Otherwise, attempts to discover a valid CUDA toolkit location and return.
    If not found, return None.
    """
    # Check if the environment variable is already set, if so, return it immediately.
    try:
        cuda_toolkit_path_existing = get_env_var("CUDA_TOOLKIT_PATH")
        if cuda_toolkit_path_existing:
            return cuda_toolkit_path_existing

        found_cuda_home = _find_cuda_home()
        if found_cuda_home:
            return found_cuda_home
    except Exception as e:
        log().info("default_env: exception on get_cuda_toolkit_path", e)
    return None


def get_prefix_dsl_libs(prefix: str):
    """
    Returns get_env_var('{prefix}_LIBS') if set.
    Otherwise, attempts to discover libs based on heuristics and return
    If not found, return None.
    """
    # Check if the environment variable is already set, if so, return it immediately.
    try:
        prefix_libs_existing = get_env_var(f"{prefix}_LIBS")
        if prefix_libs_existing:
            return prefix_libs_existing

        def get_libs_cand(start):
            target_libs = {
                "mlir_c_runner_utils",
                "mlir_runner_utils",
                "mlir_cuda_runtime",
            }
            lib_folder_guesses = [
                "lib",
                # {$nv-internal-release begin}
                "third_party/llvm/lib",
                "llvm-prebuilt/lib",
                # {$nv-internal-release end}
            ]

            libs_cand = find_libs_in_ancestors(start, target_libs, lib_folder_guesses)
            if libs_cand:
                dsl_libs = ":".join(libs_cand)
                return dsl_libs

            return None

        # find from install folder
        dsl_libs = get_libs_cand(__file__)

        if not dsl_libs:
            # try to find from build folder structure
            dsl_libs = get_libs_cand(Path(__file__).parent.parent.resolve())

        return dsl_libs

    except Exception as e:
        log().info(f"default_env: exception on get_prefix_dsl_libs", e)
    return None


class EnvironmentVarManager:
    """Manages environment variables for configuration options.

    Printing options:
    - [DSL_NAME]_LOG_TO_CONSOLE: Print logging to stderr (default: False)
    - [DSL_NAME]_PRINT_AFTER_PREPROCESSOR: Print after preprocess (default: False)
    - [DSL_NAME]_PRINT_IR: Print generated IR (default: False)
    - [DSL_NAME]_PRINT_IR_AFTER_ALL: Enable printing the IR before and after pass execution (default: False)
    - [DSL_NAME]_FILTER_STACKTRACE: Filter internal stacktrace (default: True)
    # {$nv-internal-release begin}
    # not available in Release builds due to upstream
    - [DSL_NAME]_PRINT_PTX: Print the generated PTX to stderr (default: False)
    - [DSL_NAME]_PRINT_PTXAS_LOG: Print generated ptxas compilation to stderr (default: False)
    - [DSL_NAME]_PRINT_SASS: Print generated SASS to stderr (default: False)
    # {$nv-internal-release end}

    File options:
    - [DSL_NAME]_KEEP_IR: Save generated IR in a file (default: False)
    - [DSL_NAME]_KEEPIR_AFTER_PASSES: Save generated IR after applying the passes into a file (default: ""). Example: keepIRAfterPasses="canonicalize,cse"
    - [DSL_NAME]_LOG_TO_FILE: Store all logging into a file, excluding COMPILE_LOGS (default: False)
    # {$nv-internal-release begin}
    # not available in Release builds due to upstream
    - [DSL_NAME]_SAVE_COMPILE_OUTPUT: Store compile output (PRINT_PTX, PRINT_SASS if enabled) into a file, one per kernel (default: False)
    # {$nv-internal-release end}
    Other options:
    - [DSL_NAME]_LOG_LEVEL: Logging level to set, for LOG_TO_CONSOLE or LOG_TO_FILE (default: 1).
    - [DSL_NAME]_DRYRUN: Generates IR only (default: False)
    - [DSL_NAME]_TEST_MODE: Print IR after passes, stop after compilation (default: False)
    - [DSL_NAME]_ARCH: GPU architecture (default: "sm_100")
    - [DSL_NAME]_WARNINGS_AS_ERRORS: Enable warnings as error (default: False)
    - [DSL_NAME]_WARNINGS_IGNORE: Ignore warnings (default: False)
    - [DSL_NAME]_JIT_TIME_PROFILING: Whether or not to profile the IR generation/compilation/execution time (default: False)
    - [DSL_NAME]_DISABLE_FILE_CACHING: Disable file caching (default: False)
    - [DSL_NAME]_FILE_CACHING_CAPACITY: Limits the number of the cache save/load files (default: 1000)
    - [DSL_NAME]_LIBS: Path to shared libraries (default: None)
    - [DSL_NAME]_NO_SOURCE_LOCATION: Generate source location (default: False)
    # {$nv-internal-release begin}
    - [DSL_NAME]_PIPELINE: MLIR pipeline
    # {$nv-internal-release end}

    """

    def __init__(self, prefix="DSL"):
        self.prefix = prefix  # change if needed

        # Printing options
        self.log_to_console = get_env_var(f"{prefix}_LOG_TO_CONSOLE", False)
        self.print_after_preprocessor = get_env_var(
            f"{prefix}_PRINT_AFTER_PREPROCESSOR", False
        )
        self.printIR = get_env_var(f"{prefix}_PRINT_IR", False)
        self.enablePassesPrinting = get_env_var(f"{prefix}_PRINT_IR_AFTER_ALL", False)
        self.filterStacktrace = get_env_var(f"{prefix}_FILTER_STACKTRACE", True)
        # {$nv-internal-release begin}
        self.printPTX = get_env_var(f"{prefix}_PRINT_PTX", False)
        self.printPTXASlog = get_env_var(f"{prefix}_PRINT_PTXAS_LOG", False)
        self.printSASS = get_env_var(f"{prefix}_PRINT_SASS", False)
        # {$nv-internal-release end}

        # File options
        self.keepIR = get_env_var(f"{prefix}_KEEP_IR", False)
        self.keepIRAfterPasses = get_env_var(f"{prefix}_KEEPIR_AFTER_PASSES")
        self.log_to_file = get_env_var(f"{prefix}_LOG_TO_FILE", False)
        # {$nv-internal-release begin}
        self.saveCompileOutput = get_env_var(f"{prefix}_SAVE_COMPILE_OUTPUT", False)
        # {$nv-internal-release end}

        # Other options
        self.log_level = get_env_var(f"{prefix}_LOG_LEVEL", 1)
        self.dryrun = get_env_var(f"{prefix}_DRYRUN", False)
        self.test_mode = get_env_var(f"{prefix}_TEST_MODE", False)
        self.arch = get_env_var(f"{prefix}_ARCH", detect_gpu_arch(prefix))
        self.warnings_as_errors = get_env_var(f"{prefix}_WARNINGS_AS_ERRORS", False)
        self.warnings_ignore = get_env_var(f"{prefix}_WARNINGS_IGNORE", False)
        self.jitTimeProfiling = get_env_var(f"{prefix}_JIT_TIME_PROFILING", False)
        self.disable_file_caching = get_env_var(f"{prefix}_DISABLE_FILE_CACHING", False)
        self.file_caching_capacity = get_env_var(
            f"{prefix}_FILE_CACHING_CAPACITY", 1000
        )
        self.generate_source_location = not get_env_var(
            f"{prefix}_NO_SOURCE_LOCATION", False
        )
        # {$nv-internal-release begin}
        self.pipeline = get_env_var(f"{prefix}_PIPELINE")
        # {$nv-internal-release end}

        # set cuda
        self.cuda_toolkit = get_cuda_toolkit_path()

        # set mlir shared libraries
        self.shared_libs = get_prefix_dsl_libs(prefix)
