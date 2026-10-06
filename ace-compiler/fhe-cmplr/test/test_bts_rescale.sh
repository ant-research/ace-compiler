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

function test_relu_resbm()
{
  bld_dir=$1
  ${bld_dir}/driver/fhe_cmplr -CKKS:sbm:mxbl=13:tia ./relu.onnx
  bts_cnt=`grep CKKS.bootstrap relu.t | wc -l`
  if [ ${bts_cnt} -ne 0 ]; then
    echo "ERROR: redundant bootstrap"
    exit 1
  fi
  rs_cnt=`grep CKKS.rescale relu.t | wc -l`
  if [ ${rs_cnt} -gt 41 ]; then
    echo "ERROR: redundant rescale"
    exit 1
  fi
  lvl0_cnt=`grep "\[level=0" relu.t | wc -l`
  if [ ${lvl0_cnt} -gt 0 ]; then
    echo "ERROR: node with invalid level"
    exit 1
  fi
}

function test_relu_resbm_li8()
{
  bld_dir=$1
  ${bld_dir}/driver/fhe_cmplr -CKKS:sbm:mxbl=8:icl=8:tia ./relu.onnx
  bts_cnt=`grep CKKS.bootstrap relu.t | wc -l`
  if [ ${bts_cnt} -ne 1 ]; then
    echo "ERROR: redundant bootstrap"
    exit 1
  fi
  bts_lev=`grep CKKS.bootstrap relu.t | grep "level=5" | wc -l`
  if [ ${bts_lev} -ne 1 ]; then
    echo "ERROR: bootstrap resulting level must be 5"
    exit 1
  fi
  rs_cnt=`grep CKKS.rescale relu.t | wc -l`
  if [ ${rs_cnt} -gt 41 ]; then
    echo "ERROR: redundant rescale"
    exit 1
  fi
  lvl0_cnt=`grep "\[level=0" relu.t | wc -l`
  if [ ${lvl0_cnt} -gt 0 ]; then
    echo "ERROR: node with invalid level"
    exit 1
  fi
}

function test_resnet20_cifar10()
{
  bld_dir=$1
  VEC_OPT=""
  SIHE_OPT="-SIHE:relu_vr_def=2.0"
  CKKS_OPT="-CKKS:mxbl=16:mbc=2:sbm:tsbp"
  P2C_OPT="-P2C:df=tmp.data"
  ${bld_dir}/driver/fhe_cmplr $VEC_OPT $SIHE_OPT $CKKS_OPT $P2C_OPT ./resnet20_cifar10.onnx
  grep MIN ./resnet20_cifar10.t > bts_res_lev.log
  diff_cnt=`diff bts_res_lev.log resnet20_cifar10.log | wc -l`
  if [ ${diff_cnt} -eq 0 ]; then
    exit 0
  else
    echo "bootstrap resulting level of resnet20_cifar10 changed:"
    diff bts_res_lev.log resnet20_cifar10.log
    exit 1
  fi
}

function test_conv2d()
{
  bld_dir=$1
  fn=conv2d
  ${bld_dir}/driver/fhe_cmplr -CKKS:tia ./${fn}.onnx
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
  ms_cnt=`grep CKKS.modswitch ${fn}.t | wc -l`
  if [ ${ms_cnt} -ne 0 ]; then
    echo "ERROR:redundant modswitch"
    exit 1
  fi
  lvl0_cnt=`grep "\[level=0" ${fn}.t | wc -l`
  if [ ${lvl0_cnt} -gt 0 ]; then
    echo "ERROR: node with invalid level"
    exit 1
  fi
}

function test_conv2d_resbm()
{
  bld_dir=$1
  fn=conv2d
  ${bld_dir}/driver/fhe_cmplr -CKKS:sbm:mxbl=10:tia ./${fn}.onnx
  bts_cnt=`grep CKKS.bootstrap ${fn}.t | wc -l`
  if [ ${bts_cnt} -ne 0 ]; then
    echo "ERROR: redundant bootstrap"
    exit 1
  fi
  rs_cnt=`grep CKKS.rescale ${fn}.t | wc -l`
  if [ ${rs_cnt} -gt 4 ]; then
    echo "ERROR: redundant rescale"
    exit 1
  fi
  lvl0_cnt=`grep "\[level=0" ${fn}.t | wc -l`
  if [ ${lvl0_cnt} -gt 0 ]; then
    echo "ERROR: node with invalid level"
    exit 1
  fi
}

echo "Download onnx file and binary SIHE IR from oss://antsys-fhe/cti/ir/sihe"
ossutil64 cp oss://antsys-fhe/cti/ir/sihe/ sihe/ -r --update

dir_cti=`pwd`
dir_sihe=${dir_cti}/sihe
cd $dir_sihe

test_relu $1
test_relu_resbm $1
test_relu_resbm_li8 $1
test_conv2d $1
test_conv2d_resbm $1
test_resnet20_cifar10 $1
cd $dir_cti
