#!/bin/bash
ONNX_DIR="/app/model"
WORK_DIR="/app/test"
CMPLR_DIR="/app/hpao_cmplr"
INCLUDE="-I$CMPLR_DIR/rtlib/include -I$CMPLR_DIR/rtlib/include/rt_ant"
LIB_DIR="$CMPLR_DIR/rtlib/lib"

onnx_file=$1
main_file=$WORK_DIR/${onnx_file%onnx*}"c"
graph_file="${onnx_file%onnx*}onnx.c"
exe_file="${onnx_file}.out"
cmplr_type=$2
full_logname=$3

cd $WORK_DIR
rm ${onnx}.out -rf
rm ./${exe_file}

#if [[ -n "$cmplr_type" && $cmplr_type == "build_rel" ]] ; then
#  lib_dir="build_rel"
#elif [[ -n "$cmplr_type" && $cmplr_type == "build_rel_omp" ]]; then
#  lib_dir="build_rel_omp"
#else
#  lib_dir="build_debug"
#fi
lib_dir="hpao_cmplr"

if [ -z $full_logname ]; then
  logname=`date +'%Y-%m-%d_%H:%M'`
  full_logname="logs/${exe_file}.$logname.$lib_dir.log"
fi

echo  "$lib_dir $onnx_file $main_file $graph_file" >> $full_logname

cc $main_file ${graph_file} $INCLUDE $LIB_DIR/libFHErt_ant.a $LIB_DIR/libFHErt_common.a /usr/lib/x86_64-linux-gnu/libgmp.so /usr/lib/x86_64-linux-gnu/libm.so -o ${exe_file} -O3 -g -fopenmp  >> $full_logname  2>&1 #-lhexl 
export RTLIB_BTS_EVEN_POLY=1
export RTLIB_TIMING_OUTPUT=stdout
export RT_DATA_SYNC_READ=1
export PT_ENTRY_COUNT=16
export PT_PREFETCH_COUNT=8
/usr/bin/time -f "%es %MKB" ./${exe_file} >> $full_logname 2>&1
ret=$?
echo "Exit $ret - Check log: $full_logname"
exit $ret
