#!/bin/bash

echo "parameter : $1"

echo "Download onnx file from oss://antsys-fhe/cti/onnx/"
ossutil64 cp oss://antsys-fhe/cti/onnx/op/conv_p1s1_relu.onnx onnx/ --update
ossutil64 cp oss://antsys-fhe/cti/onnx/base/small_gemm_relu.onnx onnx/ --update
ossutil64 cp oss://antsys-fhe/cti/onnx/cnn/resnet_block1_with_relu.onnx onnx/ --update

expect_cnt=0
for item in `ls ./onnx/*.onnx`
do
  echo $item
  if [[ "$item" == "conv_p1s1_relu.onnx" ]]; then
    expect_cnt = 1
  elif [[ "$item" == "small_gemm_relu.onnx" ]]; then
    expect_cnt = 1
  elif [[ "$item" == "resnet_block1_with_relu.onnx" ]]; then
    expect_cnt = 2
  fi
  t_file=${item%onnx*}"t"
  echo "======================================="
  ../driver/onnx_cmplr -VEC:conv_fast:mf:tia $item
  ma_cnt = $(grep ATTR\\[mask= ${t_file} | wc -l)
  if [ ${ma_cnt} -ne ${expect_cnt} ]; then
    echo "ERROR:  mask fusion failed"
    exit 1
  fi
done
