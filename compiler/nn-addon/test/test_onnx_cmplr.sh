#!/bin/bash

echo "parameter : $1"

echo "Download onnx file from oss://antsys-fhe/cti/onnx/"
ossutil64 cp oss://antsys-fhe/cti/onnx/op/ onnx/ -r --update

for item in `ls ./onnx/*.onnx`
do
  echo $item
  echo "======================================="
  ../driver/onnx_cmplr $item
done
