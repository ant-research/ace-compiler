#!/bin/bash
set -x

export RTLIB_TIMING_OUTPUT=-
export RTLIB_BTS_EVEN_POLY=1
export OMP_NUM_THREADS=8

ID=1104
TEST_CASE=squeezenet_cifar10
ONNX_NAME=${TEST_CASE}.onnx
CASE_DIR="${WRK_DIR}/ace-compiler/fhe-cmplr/rtlib/ant/dataset"
SRC_FILE="${CASE_DIR}/${TEST_CASE}.cxx"
INC_FILE="${CASE_DIR}/${ONNX_NAME}.inc"

RELU_VR="/conv1/conv1.2/Relu=2"    # 1
RELU_VR="$RELU_VR;/conv10/conv10.2/Relu=11"    # 2
RELU_VR="$RELU_VR;/fire2/Relu=3"    # 3
RELU_VR="$RELU_VR;/fire2/squeeze/squeeze.2/Relu=2"    # 4
RELU_VR="$RELU_VR;/fire3/Relu=4"    # 5
RELU_VR="$RELU_VR;/fire3/squeeze/squeeze.2/Relu=3"    # 6
RELU_VR="$RELU_VR;/fire4/Relu=3"    # 7
RELU_VR="$RELU_VR;/fire4/squeeze/squeeze.2/Relu=4"    # 8
RELU_VR="$RELU_VR;/fire5/Relu=3"    # 9
RELU_VR="$RELU_VR;/fire5/squeeze/squeeze.2/Relu=4"    # 10
RELU_VR="$RELU_VR;/fire6/Relu=3"    # 11
RELU_VR="$RELU_VR;/fire6/squeeze/squeeze.2/Relu=4"    # 12
RELU_VR="$RELU_VR;/fire7/Relu=3"    # 13
RELU_VR="$RELU_VR;/fire7/squeeze/squeeze.2/Relu=4"    # 14
RELU_VR="$RELU_VR;/fire8/Relu=3"    # 15
RELU_VR="$RELU_VR;/fire8/squeeze/squeeze.2/Relu=5"    # 16
RELU_VR="$RELU_VR;/fire9/Relu=13"    # 17
RELU_VR="$RELU_VR;/fire9/squeeze/squeeze.2/Relu=5"    # 18
RELU_VR_DEF=2

#CMPLR_OPTS="-CKKS:sk_hw=192:q0=60:sf=56:resbm -VEC:conv_fast:gemm_fast"
CMPLR_OPTS="-CKKS:hw=192:q0=60:sf=57"
SIHE_OPTS="-SIHE:rtt:relu_vr_def=${RELU_VR_DEF}:relu_vr=${RELU_VR}"
P2C_OPTS="-P2C:df=${WRK_DIR}/model/${ONNX_NAME}.${ID}.msg:fp"

$2/bin/fhe_cmplr ${WRK_DIR}/model/$ONNX_NAME $CMPLR_OPTS $SIHE_OPTS $P2C_OPTS -o $INC_FILE
[ $? -ne 0 ] && echo "fhe_cmplr failed. exit" && exit 1

cp ${WRK_DIR}/model/${TEST_CASE}.cxx $SRC_FILE
[ $? -ne 0 ] && echo "add case ${TEST_CASE} failed. exit" && exit 1

#make $TEST_CASE
c++ $SRC_FILE \
	-DRTLIB_SUPPORT_LINUX \
	-I $2/include \
	-I $2/rtlib/include/ \
	-I $2/rtlib/include/ant/ \
	-O3 -DNDEBUG -std=gnu++17 -fopenmp \
	$2/rtlib/lib/libFHErt_ant.a \
	$2/rtlib/lib/libFHErt_common.a \
	$2/lib/libAIRutil.a \
	-lgmp -lm -o $TEST_CASE -lgomp
[ $? -ne 0 ] && echo "linke ${TEST_CASE} failed. exit" && exit 1

exit 0
