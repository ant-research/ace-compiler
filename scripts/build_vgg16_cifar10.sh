#!/bin/bash
set -x

export RTLIB_TIMING_OUTPUT=-
export RTLIB_BTS_EVEN_POLY=1
export OMP_NUM_THREADS=8

ID=1104
TEST_CASE=vgg16_cifar10
ONNX_NAME=${TEST_CASE}.onnx
CASE_DIR="${WRK_DIR}/ace-compiler/fhe-cmplr/rtlib/ant/dataset"
SRC_FILE="${CASE_DIR}/${TEST_CASE}.cxx"
INC_FILE="${CASE_DIR}/${ONNX_NAME}.inc"


RELU_VR="/features/features.12/Relu=4"    # 1
RELU_VR="$RELU_VR;/features/features.16/Relu=4"    # 2
RELU_VR="$RELU_VR;/features/features.19/Relu=4"    # 3
RELU_VR="$RELU_VR;/features/features.2/Relu=5"    # 4
RELU_VR="$RELU_VR;/features/features.22/Relu=4"    # 5
RELU_VR="$RELU_VR;/features/features.26/Relu=4"    # 6
RELU_VR="$RELU_VR;/features/features.29/Relu=4"    # 7
RELU_VR="$RELU_VR;/features/features.32/Relu=4"    # 8
RELU_VR="$RELU_VR;/features/features.36/Relu=4"    # 9
RELU_VR="$RELU_VR;/features/features.39/Relu=4"    # 10
RELU_VR="$RELU_VR;/features/features.42/Relu=4"    # 11
RELU_VR="$RELU_VR;/features/features.5/Relu=5"    # 12
RELU_VR="$RELU_VR;/features/features.9/Relu=4"    # 13
RELU_VR_DEF=4

# CMPLR_OPTS="-CKKS:sk_hw=192:q0=60:sf=56:rtt:resbm -VEC:conv_fast:gemm_fast:rtt:rtv"
CMPLR_OPTS="-CKKS:hw=192:q0=60:sf=56:rtt -VEC:rtt"
SIHE_OPTS="-SIHE:rtt:relu_vr_def=${RELU_VR_DEF}:relu_vr=${RELU_VR}:rtt"
P2C_OPTS="-P2C:df=${WRK_DIR}/model/${ONNX_NAME}.${ID}.msg:fp"

$2/bin/fhe_cmplr ${WRK_DIR}/model/$ONNX_NAME $CMPLR_OPTS $SIHE_OPTS $P2C_OPTS -o $INC_FILE
[ $? -ne 0 ] && echo "fhe_cmplr failed. exit" && exit 1

cp ${WRK_DIR}/model/${TEST_CASE}.cxx $SRC_FILE
[ $? -ne 0 ] && echo "add case ${TEST_CASE} failed. exit" && exit 1

#make $TEST_CASE -j8
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