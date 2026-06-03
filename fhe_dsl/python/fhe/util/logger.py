#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Logging utilities for ACE FHE.

Provides a unified logging configuration with:
- Configurable log levels
- Consistent formatting
- Module-specific loggers
- Optional file logging

Usage:
    from ace.fhe.util.logger import get_logger, setup_logging

    # Setup logging (call once at application startup)
    setup_logging(level=logging.INFO)  # or logging.DEBUG for verbose

    logger = get_logger(__name__)
    logger.info("Information message")
    logger.debug("Debug message")
    logger.warning("Warning message")
    logger.error("Error message")

Log Level Guide:
    - CRITICAL (50): System errors, crashes
    - ERROR (40): Operation failures that need attention
    - WARNING (30): Unexpected but recoverable situations
    - INFO (20): Normal progress, milestones
    - DEBUG (10): Detailed debugging information

To change log level:
    # Via environment variable (recommended)
    export ACE_LOG_LEVEL=INFO

    # Or programmatically
    setup_logging(level=logging.INFO)
    set_log_level(logging.INFO, "ace.fhe.frontend.torch")
"""

import logging
import os
from pathlib import Path
from typing import Optional, Dict, Any


# Default log format
DEFAULT_LOG_FORMAT = "[%(levelname)s] %(name)s: %(message)s"
VERBOSE_LOG_FORMAT = "%(asctime)s [%(levelname)s] %(name)s: %(message)s"
DEFAULT_DATE_FORMAT = "%Y-%m-%d %H:%M:%S"

# Default log level for development (can be overridden by ACE_LOG_LEVEL env var)
# Set to DEBUG by default for development; use ACE_LOG_LEVEL=INFO for quieter output
DEFAULT_LOG_LEVEL = logging.DEBUG


def _get_log_level_from_env() -> int:
    """
    Get log level from ACE_LOG_LEVEL environment variable.

    Priority:
    1. ACE_LOG_LEVEL env var (if set)
    2. DEFAULT_LOG_LEVEL (DEBUG by default)
    """
    level_str = os.environ.get("ACE_LOG_LEVEL", "").upper()
    level_map = {
        "DEBUG": logging.DEBUG,
        "INFO": logging.INFO,
        "WARNING": logging.WARNING,
        "WARN": logging.WARNING,
        "ERROR": logging.ERROR,
        "CRITICAL": logging.CRITICAL,
    }
    if level_str in level_map:
        return level_map[level_str]

    # Fall back to default
    return DEFAULT_LOG_LEVEL


# Track initialization state
_initialized = False
_console_handler = None


def setup_logging(
    level: Optional[int] = None,
    log_format: str = DEFAULT_LOG_FORMAT,
    date_format: str = DEFAULT_DATE_FORMAT,
    enable_file_logging: bool = False,
    log_dir: Optional[str] = None,
    module_log_configs: Optional[Dict[str, Dict[str, Any]]] = None,
) -> None:
    """
    Setup logging configuration.

    Call this once at application startup. Can be called multiple times
    to reconfigure logging.

    Args:
        level: Root logger level. If None, uses ACE_LOG_LEVEL env var or DEFAULT_LOG_LEVEL.
        log_format: Log format string. Use VERBOSE_LOG_FORMAT for timestamps.
        date_format: Date format string.
        enable_file_logging: Enable file logging (default: False).
        log_dir: Directory for log files (default: ./logs).
        module_log_configs: Per-module file logging config:
            {
                "ace.fhe.driver": {"file": "driver.log", "level": "DEBUG"},
                "ace.fhe.runtime": {"file": "runtime.log", "level": "INFO"},
            }

    Examples:
        # Simple setup - DEBUG level (default)
        setup_logging()

        # INFO level for quieter output
        setup_logging(level=logging.INFO)

        # Or via environment variable
        export ACE_LOG_LEVEL=INFO

        # With file logging
        setup_logging(enable_file_logging=True, log_dir="/tmp/ace_logs")

        # Per-module file logging
        setup_logging(module_log_configs={
            "ace.fhe.frontend.torch": {"file": "frontend.log", "level": "DEBUG"},
        })
    """
    global _initialized, _console_handler

    # Get level from env if not specified
    if level is None:
        level = _get_log_level_from_env()

    # Get root logger
    root_logger = logging.getLogger()
    root_logger.setLevel(logging.DEBUG)  # Capture all levels; handlers filter

    # Remove existing handlers to reconfigure
    for handler in root_logger.handlers[:]:
        root_logger.removeHandler(handler)

    # Console handler
    console_handler = logging.StreamHandler()
    console_handler.setLevel(level)
    console_formatter = logging.Formatter(fmt=log_format, datefmt=date_format)
    console_handler.setFormatter(console_formatter)
    root_logger.addHandler(console_handler)
    _console_handler = console_handler

    # File logging (optional)
    if enable_file_logging:
        log_dir = log_dir or "logs"
        Path(log_dir).mkdir(parents=True, exist_ok=True)

        # Main log file
        main_log = Path(log_dir) / "ace_fhe.log"
        file_handler = logging.FileHandler(main_log)
        file_handler.setLevel(logging.DEBUG)
        file_handler.setFormatter(console_formatter)
        root_logger.addHandler(file_handler)

        # Per-module log files
        if module_log_configs:
            for logger_name, config in module_log_configs.items():
                log_file = Path(log_dir) / config.get("file", f"{logger_name}.log")
                module_level = getattr(logging, config.get("level", "DEBUG").upper())

                module_logger = logging.getLogger(logger_name)
                module_logger.setLevel(module_level)
                module_logger.propagate = False  # Don't duplicate to root

                module_handler = logging.FileHandler(log_file)
                module_handler.setLevel(module_level)
                module_handler.setFormatter(console_formatter)
                module_logger.addHandler(module_handler)

    _initialized = True


def get_logger(name: str, level: Optional[int] = None) -> logging.Logger:
    """
    Get or create a logger with the given name.

    Args:
        name: Logger name (typically __name__)
        level: Log level for this logger (default: inherits from root)

    Returns:
        Configured logger instance

    Examples:
        logger = get_logger(__name__)
        logger = get_logger("ace.fhe.frontend.torch", level=logging.DEBUG)
    """
    logger = logging.getLogger(name)

    # Set level if provided
    if level is not None:
        logger.setLevel(level)

    return logger


def set_log_level(level: int, logger_name: Optional[str] = None) -> None:
    """
    Set log level for a specific logger or all loggers.

    Args:
        level: Logging level (e.g., logging.DEBUG, logging.INFO)
        logger_name: Logger name (None for root logger)

    Examples:
        # Set all loggers to DEBUG
        set_log_level(logging.DEBUG)

        # Set specific module to DEBUG
        set_log_level(logging.DEBUG, "ace.fhe.frontend.torch")
    """
    if logger_name:
        logging.getLogger(logger_name).setLevel(level)
    else:
        # Set console handler level
        if _console_handler:
            _console_handler.setLevel(level)
        logging.root.setLevel(level)


def enable_debug_logging(module_name: Optional[str] = None) -> None:
    """
    Enable DEBUG level logging.

    Args:
        module_name: Specific module to enable debug for, or None for all.

    Examples:
        enable_debug_logging()  # All modules
        enable_debug_logging("ace.fhe.frontend.torch")  # Specific module
    """
    set_log_level(logging.DEBUG, module_name)


def disable_debug_logging() -> None:
    """Disable debug logging (set back to INFO)."""
    set_log_level(logging.INFO)


# Convenience loggers for common modules
def get_frontend_logger() -> logging.Logger:
    """Get the frontend logger."""
    return get_logger("ace.fhe.frontend")


def get_ir_logger() -> logging.Logger:
    """Get the IR logger."""
    return get_logger("ace.fhe.ir")


def get_torch_frontend_logger() -> logging.Logger:
    """Get the torch frontend logger."""
    return get_logger("ace.fhe.frontend.torch")


def get_onnx_frontend_logger() -> logging.Logger:
    """Get the ONNX frontend logger."""
    return get_logger("ace.fhe.frontend.onnx")


def get_driver_logger() -> logging.Logger:
    """Get the driver logger."""
    return get_logger("ace.fhe.driver")


def get_runtime_logger() -> logging.Logger:
    """Get the runtime logger."""
    return get_logger("ace.fhe.runtime")


# ======================
# Backward Compatibility
# ======================

def setup_fhe_logger():
    """
    Initialize FHE logger with default configuration.

    Deprecated: Use setup_logging() instead.

    Log level is controlled by:
    1. ACE_LOG_LEVEL environment variable (if set)
    2. DEFAULT_LOG_LEVEL (DEBUG by default)
    """
    setup_logging(
        level=None,  # Use ACE_LOG_LEVEL env var or DEFAULT_LOG_LEVEL
        module_log_configs={
            "ace.fhe.driver": {
                "file": "fhe_driver.log",
                "level": "DEBUG"
            },
            "ace.fhe.runtime": {
                "file": "fhe_execute.log",
                "level": "DEBUG"
            }
        }
    )


# Auto-setup if ACE_LOG_LEVEL is set
if os.environ.get("ACE_LOG_LEVEL"):
    setup_logging()