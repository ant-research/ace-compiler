#!/bin/bash -x

echo "parameter : $1"

function test_relu()
{
  bld_dir=$1
  ${bld_dir}/driver/fhe_cmplr -SIHE:b2ir=./relu.onnx.sihe -CKKS:tia ./relu.onnx
  bts_cnt=`grep CKKS.bootstrap relu.t | wc -l`
  if [ ${bts_cnt} -ne 0 ]; then
    echo "ERROR: redundant bootstrap"
    exit 1
  fi
  rs_cnt=`grep CKKS.rescale relu.t | wc -l`
  if [ ${rs_cnt} -gt 63 ]; then
    echo "ERROR: redundant rescale"
    exit 1
  fi
}

function test_conv2d()
{
  bld_dir=$1
  fn=conv2d
  ${bld_dir}/driver/fhe_cmplr -SIHE:b2ir=./${fn}.onnx.sihe -CKKS:tia ./${fn}.onnx
  bts_cnt=`grep CKKS.bootstrap ${fn}.t | wc -l`
  if [ ${bts_cnt} -ne 0 ]; then
    echo "ERROR: redundant bootstrap"
    exit 1
  fi
  rs_cnt=`grep CKKS.rescale ${fn}.t | wc -l`
  if [ ${rs_cnt} -gt 5 ]; then
    echo "ERROR: redundant rescale"
    exit 1
  fi
}

echo "Download onnx file and binary SIHE IR from oss://antsys-fhe/cti/ir/sihe"
ossutil64 cp oss://antsys-fhe/cti/ir/sihe/ sihe/ -r --update

dir_cti=`pwd`
dir_sihe=${dir_cti}/sihe
cd $dir_sihe

test_relu $1
test_conv2d $1

cd $dir_cti
