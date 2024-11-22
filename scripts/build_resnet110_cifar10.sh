#!/bin/bash

TEST_NAME=resnet110_cifar10
ONNX_NAME=${TEST_NAME}_train.onnx
WRK_DIR=`pwd`

[ $# -ne 1 ] && echo "[WARN] Usage: $0 <cfg_name>. For example: $0 test" && exit 0
[ ! -f driver/fhe_cmplr ] && echo "[ERROR] Run $o under the build directory" && exit 1
[ ! -f ../fhe-cmplr/rtlib/ant/dataset/$TEST_NAME.cxx ] && echo "[ERROR] failed to locate $TEST_NAME main file" && exit 1
[ ! -f ../model/$ONNX_NAME ] && echo "[ERROR] failed to locate ../model/$ONNX_NAME" && exit 1

ID=$1
Q0=60
SF=56

RELU_VR_DEF=3
RELU_VR="/relu/Relu=14"    # 109
RELU_VR="$RELU_VR;/layer1/layer1.0/relu/Relu=4"    # 1
RELU_VR="$RELU_VR;/layer1/layer1.0/relu_1/Relu=14"    # 2
RELU_VR="$RELU_VR;/layer1/layer1.1/relu/Relu=4"    # 3
RELU_VR="$RELU_VR;/layer1/layer1.1/relu_1/Relu=15"    # 4
RELU_VR="$RELU_VR;/layer1/layer1.10/relu/Relu=5"    # 5
RELU_VR="$RELU_VR;/layer1/layer1.10/relu_1/Relu=18"    # 6
RELU_VR="$RELU_VR;/layer1/layer1.11/relu/Relu=5"    # 7
RELU_VR="$RELU_VR;/layer1/layer1.11/relu_1/Relu=17"    # 8
RELU_VR="$RELU_VR;/layer1/layer1.12/relu/Relu=4"    # 9
RELU_VR="$RELU_VR;/layer1/layer1.12/relu_1/Relu=21"    # 10
RELU_VR="$RELU_VR;/layer1/layer1.13/relu/Relu=7"    # 11
RELU_VR="$RELU_VR;/layer1/layer1.13/relu_1/Relu=22"    # 12
RELU_VR="$RELU_VR;/layer1/layer1.14/relu/Relu=7"    # 13
RELU_VR="$RELU_VR;/layer1/layer1.14/relu_1/Relu=23"    # 14
RELU_VR="$RELU_VR;/layer1/layer1.15/relu/Relu=7"    # 15
RELU_VR="$RELU_VR;/layer1/layer1.15/relu_1/Relu=25"    # 16
RELU_VR="$RELU_VR;/layer1/layer1.16/relu/Relu=6"    # 17
RELU_VR="$RELU_VR;/layer1/layer1.16/relu_1/Relu=22"    # 18
RELU_VR="$RELU_VR;/layer1/layer1.17/relu/Relu=6"    # 19
RELU_VR="$RELU_VR;/layer1/layer1.17/relu_1/Relu=22"    # 20
RELU_VR="$RELU_VR;/layer1/layer1.2/relu/Relu=3"    # 21
RELU_VR="$RELU_VR;/layer1/layer1.2/relu_1/Relu=15"    # 22
RELU_VR="$RELU_VR;/layer1/layer1.3/relu/Relu=6"    # 23
RELU_VR="$RELU_VR;/layer1/layer1.3/relu_1/Relu=20"    # 24
RELU_VR="$RELU_VR;/layer1/layer1.4/relu/Relu=5"    # 25
RELU_VR="$RELU_VR;/layer1/layer1.4/relu_1/Relu=17"    # 26
RELU_VR="$RELU_VR;/layer1/layer1.5/relu/Relu=5"    # 27
RELU_VR="$RELU_VR;/layer1/layer1.5/relu_1/Relu=17"    # 28
RELU_VR="$RELU_VR;/layer1/layer1.6/relu/Relu=5"    # 29
RELU_VR="$RELU_VR;/layer1/layer1.6/relu_1/Relu=17"    # 30
RELU_VR="$RELU_VR;/layer1/layer1.7/relu/Relu=4"    # 31
RELU_VR="$RELU_VR;/layer1/layer1.7/relu_1/Relu=17"    # 32
RELU_VR="$RELU_VR;/layer1/layer1.8/relu/Relu=4"    # 33
RELU_VR="$RELU_VR;/layer1/layer1.8/relu_1/Relu=17"    # 34
RELU_VR="$RELU_VR;/layer1/layer1.9/relu/Relu=5"    # 35
RELU_VR="$RELU_VR;/layer1/layer1.9/relu_1/Relu=18"    # 36
RELU_VR="$RELU_VR;/layer2/layer2.0/relu/Relu=6"    # 37
RELU_VR="$RELU_VR;/layer2/layer2.0/relu_1/Relu=14"    # 38
RELU_VR="$RELU_VR;/layer2/layer2.1/relu/Relu=4"    # 39
RELU_VR="$RELU_VR;/layer2/layer2.1/relu_1/Relu=14"    # 40
RELU_VR="$RELU_VR;/layer2/layer2.10/relu/Relu=3"    # 41
RELU_VR="$RELU_VR;/layer2/layer2.10/relu_1/Relu=19"    # 42
RELU_VR="$RELU_VR;/layer2/layer2.11/relu/Relu=3"    # 43
RELU_VR="$RELU_VR;/layer2/layer2.11/relu_1/Relu=19"    # 44
RELU_VR="$RELU_VR;/layer2/layer2.12/relu/Relu=3"    # 45
RELU_VR="$RELU_VR;/layer2/layer2.12/relu_1/Relu=22"    # 46
RELU_VR="$RELU_VR;/layer2/layer2.13/relu/Relu=3"    # 47
RELU_VR="$RELU_VR;/layer2/layer2.13/relu_1/Relu=21"    # 48
RELU_VR="$RELU_VR;/layer2/layer2.14/relu/Relu=3"    # 49
RELU_VR="$RELU_VR;/layer2/layer2.14/relu_1/Relu=23"    # 50
RELU_VR="$RELU_VR;/layer2/layer2.15/relu/Relu=3"    # 51
RELU_VR="$RELU_VR;/layer2/layer2.15/relu_1/Relu=22"    # 52
RELU_VR="$RELU_VR;/layer2/layer2.16/relu/Relu=4"    # 53
RELU_VR="$RELU_VR;/layer2/layer2.16/relu_1/Relu=23"    # 54
RELU_VR="$RELU_VR;/layer2/layer2.17/relu/Relu=3"    # 55
RELU_VR="$RELU_VR;/layer2/layer2.17/relu_1/Relu=22"    # 56
RELU_VR="$RELU_VR;/layer2/layer2.2/relu/Relu=3"    # 57
RELU_VR="$RELU_VR;/layer2/layer2.2/relu_1/Relu=15"    # 58
RELU_VR="$RELU_VR;/layer2/layer2.3/relu/Relu=4"    # 59
RELU_VR="$RELU_VR;/layer2/layer2.3/relu_1/Relu=16"    # 60
RELU_VR="$RELU_VR;/layer2/layer2.4/relu/Relu=4"    # 61
RELU_VR="$RELU_VR;/layer2/layer2.4/relu_1/Relu=16"    # 62
RELU_VR="$RELU_VR;/layer2/layer2.5/relu/Relu=3"    # 63
RELU_VR="$RELU_VR;/layer2/layer2.5/relu_1/Relu=17"    # 64
RELU_VR="$RELU_VR;/layer2/layer2.6/relu/Relu=5"    # 65
RELU_VR="$RELU_VR;/layer2/layer2.6/relu_1/Relu=17"    # 66
RELU_VR="$RELU_VR;/layer2/layer2.7/relu/Relu=3"    # 67
RELU_VR="$RELU_VR;/layer2/layer2.7/relu_1/Relu=17"    # 68
RELU_VR="$RELU_VR;/layer2/layer2.8/relu/Relu=3"    # 69
RELU_VR="$RELU_VR;/layer2/layer2.8/relu_1/Relu=18"    # 70
RELU_VR="$RELU_VR;/layer2/layer2.9/relu/Relu=4"    # 71
RELU_VR="$RELU_VR;/layer2/layer2.9/relu_1/Relu=19"    # 72
RELU_VR="$RELU_VR;/layer3/layer3.0/relu/Relu=5"    # 73
RELU_VR="$RELU_VR;/layer3/layer3.0/relu_1/Relu=14"    # 74
RELU_VR="$RELU_VR;/layer3/layer3.1/relu/Relu=4"    # 75
RELU_VR="$RELU_VR;/layer3/layer3.1/relu_1/Relu=15"    # 76
RELU_VR="$RELU_VR;/layer3/layer3.10/relu/Relu=3"    # 77
RELU_VR="$RELU_VR;/layer3/layer3.10/relu_1/Relu=20"    # 78
RELU_VR="$RELU_VR;/layer3/layer3.11/relu/Relu=4"    # 79
RELU_VR="$RELU_VR;/layer3/layer3.11/relu_1/Relu=20"    # 80
RELU_VR="$RELU_VR;/layer3/layer3.12/relu/Relu=4"    # 81
RELU_VR="$RELU_VR;/layer3/layer3.12/relu_1/Relu=20"    # 82
RELU_VR="$RELU_VR;/layer3/layer3.13/relu/Relu=4"    # 83
RELU_VR="$RELU_VR;/layer3/layer3.13/relu_1/Relu=24"    # 84
RELU_VR="$RELU_VR;/layer3/layer3.14/relu/Relu=4"    # 85
RELU_VR="$RELU_VR;/layer3/layer3.14/relu_1/Relu=27"    # 86
RELU_VR="$RELU_VR;/layer3/layer3.15/relu/Relu=4"    # 87
RELU_VR="$RELU_VR;/layer3/layer3.15/relu_1/Relu=30"    # 88
RELU_VR="$RELU_VR;/layer3/layer3.16/relu/Relu=4"    # 89
RELU_VR="$RELU_VR;/layer3/layer3.16/relu_1/Relu=27"    # 90
RELU_VR="$RELU_VR;/layer3/layer3.17/relu/Relu=9"    # 91
RELU_VR="$RELU_VR;/layer3/layer3.17/relu_1/Relu=33"    # 92
RELU_VR="$RELU_VR;/layer3/layer3.2/relu/Relu=3"    # 93
RELU_VR="$RELU_VR;/layer3/layer3.2/relu_1/Relu=15"    # 94
RELU_VR="$RELU_VR;/layer3/layer3.3/relu/Relu=4"    # 95
RELU_VR="$RELU_VR;/layer3/layer3.3/relu_1/Relu=15"    # 96
RELU_VR="$RELU_VR;/layer3/layer3.4/relu/Relu=3"    # 97
RELU_VR="$RELU_VR;/layer3/layer3.4/relu_1/Relu=15"    # 98
RELU_VR="$RELU_VR;/layer3/layer3.5/relu/Relu=3"    # 99
RELU_VR="$RELU_VR;/layer3/layer3.5/relu_1/Relu=16"    # 100
RELU_VR="$RELU_VR;/layer3/layer3.6/relu/Relu=3"    # 101
RELU_VR="$RELU_VR;/layer3/layer3.6/relu_1/Relu=16"    # 102
RELU_VR="$RELU_VR;/layer3/layer3.7/relu/Relu=3"    # 103
RELU_VR="$RELU_VR;/layer3/layer3.7/relu_1/Relu=16"    # 104
RELU_VR="$RELU_VR;/layer3/layer3.8/relu/Relu=3"    # 105
RELU_VR="$RELU_VR;/layer3/layer3.8/relu_1/Relu=17"    # 106
RELU_VR="$RELU_VR;/layer3/layer3.9/relu/Relu=3"    # 107
RELU_VR="$RELU_VR;/layer3/layer3.9/relu_1/Relu=18"    # 108

VEC_OPTS="-VEC:rtt:conv_fast"
SIHE_OPTS="-SIHE:relu_vr_def=$RELU_VR_DEF:relu_vr=$RELU_VR:relu_mul_depth=13"
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
