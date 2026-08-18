#!/bin/bash

/usr/bin/time -f "%e %M" /app/scripts/run_micro.sh --model-first > /app/micro.log 2>&1
/usr/bin/time -f "%e %M" /app/scripts/run_perf.sh --model-first > /app/model.log 2>&1
