#!/bin/bash

echo "parameter : $1"

echo "Download onnx file from oss://antsys-fhe/cti/onnx/"
ossutil64 cp oss://antsys-fhe/cti/onnx/op/ onnx/ -r --update

echo "Generate code with ant"
dir_ant="ant"
if [ ! -d "$dir_ant" ]; then
  mkdir $dir_ant
fi

for item in `ls ./onnx/*.onnx`
do
  echo $item
  echo "======================================="

  $1/driver/fhe_cmplr $item -o $dir_ant/$(basename $item).c -P2C:lib=ant
done

echo "seal lib support is not yet fully implemented"
<<COMMENT
  #echo "Generate code with seal"
  dir_seal="seal"
  if [ ! -d "$dir_seal" ]; then
    mkdir $dir_seal
  fi

  for item in `ls ./onnx/*.onnx`
  do
    echo $item
    echo "======================================="
    ./$1/driver/fhe_cmplr $item -o $dir_seal/$(basename $item).c -P2C:lib=seal
  done
COMMENT

echo "openfhe lib support is not yet fully implemented"
<<COMMENT
  #echo "Generate code with openfhe"
  dir_openfhe="openfhe"
  if [ ! -d "$dir_openfhe" ]; then
    mkdir $dir_openfhe
  fi

  for item in `ls ./onnx/*.onnx`
  do
    echo $item
    echo "======================================="
    ./$1/driver/fhe_cmplr $item -o $dir_openfhe/$(basename $item).c -P2C:lib=openfhe
  done
COMMENT
