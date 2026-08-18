#!/bin/bash

echo "parameter : $1"

echo "Download onnx file from oss://antsys-fhe/cti/onnx/"
ossutil64 cp oss://antsys-fhe/cti/onnx/cnn/resnet_gap_reshape_gemm.onnx onnx/ --update

expect_cnt=0
for item in `ls ./onnx/*.onnx`
do
  echo $item
  if [[ "$item" == "resnet_gap_reshape_gemm.onnx" ]]; then
    expect_cnt=0
  fi
  t_file=${item%onnx*}"t"
  echo "======================================="
  ../driver/onnx_cmplr -VEC:conv_fast:sss:tia $item
  ss_var_cnt = $(grep combine_cc_index ${t_file} | wc -l)
  if [ ${ss_var_cnt} -ne ${expect_cnt} ]; then
    echo "ERROR:  strided_slice fusion failed"
    exit 1
  fi
done
