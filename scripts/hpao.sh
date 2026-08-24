#!/bin/bash

/usr/bin/time -f "%e %M" /app/scripts/run_micro.sh --model-first 2>&1 | tee /app/micro.log
/usr/bin/time -f "%e %M" /app/scripts/run_perf.sh --model-first 2>&1 | tee /app/model.log
