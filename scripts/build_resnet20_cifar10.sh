#!/bin/bash
set -x

export RTLIB_TIMING_OUTPUT=-
export RTLIB_BTS_EVEN_POLY=1
export OMP_NUM_THREADS=8

ID=1104
TEST_CASE=resnet20_cifar10
ONNX_NAME=${TEST_CASE}_pre.onnx
CASE_DIR="${WRK_DIR}/ace-compiler/fhe-cmplr/rtlib/ant/dataset"
SRC_FILE="${CASE_DIR}/${TEST_CASE}.cxx"
INC_FILE="${CASE_DIR}/${ONNX_NAME}.inc"

RELU_VR="$RELU_VR;/relu/Relu=4"    #  1
RELU_VR="$RELU_VR;/layer1/layer1.0/relu_1/Relu=4"    #  3
RELU_VR="$RELU_VR;/layer1/layer1.1/relu/Relu=4"    #  4
RELU_VR="$RELU_VR;/layer1/layer1.1/relu_1/Relu=5"    #  5
RELU_VR="$RELU_VR;/layer1/layer1.2/relu_1/Relu=5"    #  7
RELU_VR="$RELU_VR;/layer2/layer2.0/relu_1/Relu=5"    #  9
RELU_VR="$RELU_VR;/layer2/layer2.1/relu_1/Relu=5"    #  11
RELU_VR="$RELU_VR;/layer2/layer2.2/relu_1/Relu=7"    #  13
RELU_VR="$RELU_VR;/layer3/layer3.0/relu_1/Relu=4"    #  15
RELU_VR="$RELU_VR;/layer3/layer3.1/relu_1/Relu=6"    #  17
RELU_VR="$RELU_VR;/layer3/layer3.2/relu/Relu=4"    #  18
RELU_VR="$RELU_VR;/layer3/layer3.2/relu_1/Relu=20"    #  19
RELU_VR_DEF=3

#CMPLR_OPTS="-CKKS:sk_hw=192:q0=60:sf=56:resbm:tia -VEC:rtt:conv_fast"
CMPLR_OPTS="-CKKS:hw=192:q0=60 -VEC:rtt"
SIHE_OPTS="-SIHE:relu_vr_def=${RELU_VR_DEF}:relu_vr=${RELU_VR}"
P2C_OPTS="-P2C:df=${WRK_DIR}/model/${ONNX_NAME}.${ID}.msg:fp"

$2/bin/fhe_cmplr ${WRK_DIR}/model/$ONNX_NAME $CMPLR_OPTS $SIHE_OPTS $P2C_OPTS -o $INC_FILE
[ $? -ne 0 ] && echo "fhe_cmplr failed. exit" && exit 1

# not need copy src file?

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

test() {
  i=$1
  while [ $i -lt $2 ]; do
    time ./dataset/$TEST_CASE  ../test/test_batch.bin $i > ${TEST_CASE}.${ID}.${i}.log 2>&1
    i=`expr $i + 1`
  done
}

test_x() {
  while [ $# -ne 0 ]; do
    time ./dataset/$TEST_CASE  ../test/test_batch.bin $1 > ${TEST_CASE}.${ID}.$1.log 2>&1
    shift
  done
}

#test_x 15  52  58  59  61  74  78  87  95  118 125 128 147 158 164 &
#test_x 169 171 178 183 188 213 221 226 228 229 232 247 269 271 275 277 &
#test_x 293 302 312 313 324 332 346 355 370 378 384 385 412 426 433 439 &
#test_x 447 456 478 508 531 562 567 577 606 618 640 665 672 680 683 685 &
#test_x 715 725 727 731 734 735 739 746 770 775 776 788 793 810 811 828 &
#test_x 836 853 882 888 893 898 910 914 923 948 953 966 972 982 994 &
