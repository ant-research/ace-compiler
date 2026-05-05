# benchmark/__init__.py
"""
ACE FHE Performance Benchmarks.

Uses pytest-benchmark for statistical performance measurement.

Usage:
    # Run all benchmarks (benchmark-only mode skips non-benchmark tests)
    pytest benchmark/ --benchmark-only -v

    # Run and save results
    pytest benchmark/ --benchmark-only --benchmark-autosave

    # Compare with previous run
    pytest benchmark/ --benchmark-only --benchmark-compare=last

    # Run specific benchmark
    pytest benchmark/test_resnet.py -k resnet20_latency -v

Results are stored in benchmark/.benchmarks/ as JSON files.
"""