#!/bin/bash

TEST_NAME=resnet56_cifar10
ONNX_NAME=${TEST_NAME}_pre.onnx
WRK_DIR=`pwd`

[ $# -ne 1 ] && echo "[WARN] Usage: $0 <cfg_name>. For example: $0 test" && exit 0
[ ! -f driver/fhe_cmplr ] && echo "[ERROR] Run $o under the build directory" && exit 1
[ ! -f ../fhe-cmplr/rtlib/ant/dataset/$TEST_NAME.cxx ] && echo "[ERROR] failed to locate $TEST_NAME main file" && exit 1
[ ! -f ../model/$ONNX_NAME ] && echo "[ERROR] failed to locate ../model/$ONNX_NAME" && exit 1

ID=$1
Q0=60
SF=56

RELU_VR_DEF=2
RELU_VR="/relu/Relu=4"    #  1
RELU_VR="$RELU_VR;/layer1/layer1.0/relu_1/Relu=6"    #  3
RELU_VR="$RELU_VR;/layer1/layer1.1/relu_1/Relu=5"    #  5
RELU_VR="$RELU_VR;/layer1/layer1.2/relu/Relu=3"    #  6
RELU_VR="$RELU_VR;/layer1/layer1.2/relu_1/Relu=6"    #  7
RELU_VR="$RELU_VR;/layer1/layer1.3/relu/Relu=3"    #  8
RELU_VR="$RELU_VR;/layer1/layer1.3/relu_1/Relu=7"    #  9
RELU_VR="$RELU_VR;/layer1/layer1.4/relu_1/Relu=6"    #  11
RELU_VR="$RELU_VR;/layer1/layer1.5/relu_1/Relu=6"    #  13
RELU_VR="$RELU_VR;/layer1/layer1.6/relu_1/Relu=6"    #  15
RELU_VR="$RELU_VR;/layer1/layer1.7/relu_1/Relu=6"    #  17
RELU_VR="$RELU_VR;/layer1/layer1.8/relu_1/Relu=5"    #  19
RELU_VR="$RELU_VR;/layer2/layer2.0/relu_1/Relu=4"    #  21
RELU_VR="$RELU_VR;/layer2/layer2.1/relu_1/Relu=4"    #  23
RELU_VR="$RELU_VR;/layer2/layer2.2/relu_1/Relu=5"    #  25
RELU_VR="$RELU_VR;/layer2/layer2.3/relu_1/Relu=5"    #  27
RELU_VR="$RELU_VR;/layer2/layer2.4/relu_1/Relu=6"    #  29
RELU_VR="$RELU_VR;/layer2/layer2.5/relu_1/Relu=8"    #  31
RELU_VR="$RELU_VR;/layer2/layer2.6/relu_1/Relu=11"    #  33
RELU_VR="$RELU_VR;/layer2/layer2.7/relu_1/Relu=11"    #  35
RELU_VR="$RELU_VR;/layer2/layer2.8/relu_1/Relu=12"    #  37
RELU_VR="$RELU_VR;/layer3/layer3.0/relu/Relu=3"    #  38
RELU_VR="$RELU_VR;/layer3/layer3.0/relu_1/Relu=5"    #  39
RELU_VR="$RELU_VR;/layer3/layer3.1/relu_1/Relu=5"    #  41
RELU_VR="$RELU_VR;/layer3/layer3.2/relu_1/Relu=5"    #  43
RELU_VR="$RELU_VR;/layer3/layer3.3/relu_1/Relu=5"    #  45
RELU_VR="$RELU_VR;/layer3/layer3.4/relu_1/Relu=5"    #  47
RELU_VR="$RELU_VR;/layer3/layer3.5/relu_1/Relu=6"    #  49
RELU_VR="$RELU_VR;/layer3/layer3.6/relu_1/Relu=8"    #  51
RELU_VR="$RELU_VR;/layer3/layer3.7/relu/Relu=3"    #  52
RELU_VR="$RELU_VR;/layer3/layer3.7/relu_1/Relu=10"    #  53
RELU_VR="$RELU_VR;/layer3/layer3.8/relu_1/Relu=12"    #  55

VEC_OPTS="-VEC:rtt:conv_fast"
SIHE_OPTS="-SIHE:relu_vr_def=$RELU_VR_DEF:relu_vr=$RELU_VR"
CKKS_OPTS="-CKKS:sk_hw=192:q0=$Q0:sf=$SF"
P2C_OPTS="-P2C:df=$WRK_DIR/$ONNX_NAME.$ID.msg:fp"

./driver/fhe_cmplr ../model/$ONNX_NAME $VEC_OPTS $SIHE_OPTS $CKKS_OPTS $P2C_OPTS
[ $? -ne 0 ] && echo "[ERROR] fhe_cmplr failed. exit" && exit 1

cp -v ${ONNX_NAME}.c ../fhe-cmplr/rtlib/ant/dataset/${ONNX_NAME}.inc
make $TEST_NAME
[ $? -ne 0 ] && echo "[ERROR] build $TEST_NAME failed. exit" && exit 1
[ ! -f ./dataset/$TEST_NAME ] && echo "[ERROR] failed to locate ./dataset/$TEST_NAME" && exit 1
cp ./dataset/$TEST_NAME ./dataset/$TEST_NAME.$ID

echo "[INFO] build $TEST_NAME succeed."
exit 0
