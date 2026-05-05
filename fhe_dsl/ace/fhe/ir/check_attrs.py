#!/usr/bin/env python3
"""Check ATTR names in generated IR."""

import logging
import sys
import subprocess
import os

# Get logger
logger = logging.getLogger(__name__)

# Find the latest model.B file
import glob
test_dirs = glob.glob('/tmp/pytest-of-root/pytest-*/test_export_success_resnet20_c0/test_export_success_resnet20_cifar10_/')
if not test_dirs:
    logger.error("No test output found")
    sys.exit(1)

latest_dir = max(test_dirs, key=os.path.getmtime)
model_file = os.path.join(latest_dir, 'model.B')

logger.info("Checking: %s", model_file)

# Dump IR - use -O2A:tia (trace IR after) to get the IR in stdout
result = subprocess.run(
    ['/root/.pyenv/lib/python3.12/site-packages/ace/bin/fhe_cmplr', '-O2A:tia', model_file],
    capture_output=True, text=True
)

logger.info("=== ATTRs from generated IR ===")
for line in result.stdout.split('\n'):
    if 'ATTR' in line:
        logger.info(line)

# Also print first 20 lines with name= for context
logger.info("=== First 20 ATTR/name lines ===")
count = 0
for line in result.stdout.split('\n'):
    if 'ATTR' in line or 'name=' in line:
        logger.info(line)
        count += 1
        if count >= 20:
            break
