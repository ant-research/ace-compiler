#!/bin/bash

TEST_NAME=$1
IMAGE_START=$2
IMAGE_END=$3
ID=acc
export WRK_DIR=`pwd`

[ $# -ne 3 ] && echo "[WARN] Usage: $0 <model_name> <start_idx> <end_idx(excluded)>. For example: $0 resnet20_cifar10 0 100" && exit 0
[ ! -d ./ace-compiler/release_omp -o ! -f ./ace-compiler/release_omp/driver/fhe_cmplr ] && echo "[ERROR] Run ./scripts/build_cmplr_omp.sh to build OpenMP-enabled FHE compiler at first." && exit 1
[ ! -d ./ace_cmplr_omp -o ! -f ./ace_cmplr_omp/bin/fhe_cmplr ] && echo "[ERROR] Run ./scripts/build_cmplr_omp.sh to install OpenMP-enabled FHE compiler at first." && exit 1
[ ! -f ./scripts/build_$TEST_NAME.sh ] && echo "[ERROR] Not find build script for $TEST_NAME. Possible model names are: resnet20_cifar10, resnet32_cifar10, resnet32_cifar100, resnet44_cifar10, resnet56_cifar10, resnet110_cifar10" && exit 1


# determine batch size and thread number
NUM_CORES=`lscpu | grep 'Core(s) per socket:' | awk '{print $4}'`
NUM_SOCKETS=`lscpu | grep 'Socket(s):' | awk '{print $2}'`
NUM_CPUS=$(($NUM_CORES * $NUM_SOCKETS))
NUM_IMAGES=$(($IMAGE_END - $IMAGE_START))
NUM_BATCH=$(( ($NUM_IMAGES / $NUM_CPUS) + (($NUM_IMAGES % $NUM_CPUS) != 0) ))
NUM_CPUS=$(( ($NUM_IMAGES / $NUM_BATCH) + (($NUM_IMAGES % $NUM_BATCH) != 0) ))

# build the test case
cp ./scripts/build_$TEST_NAME.sh ./ace-compiler/release_omp/
cd ace-compiler/release_omp
./build_$TEST_NAME.sh $ID $WRK_DIR/ace_cmplr_omp
[ $? -ne 0 ] && echo "[ERROR] failed to build $TEST_NAME" && exit 1
echo "[INFO] build $TEST_NAME succeed."

# run the test case in release_omp
if [ $TEST_NAME == resnet32_cifar100 ]; then
  DATA_FILE=$WRK_DIR/cifar/cifar-100-binary/test.bin
else
  DATA_FILE=$WRK_DIR/cifar/cifar-10-batches-bin/test_batch.bin
fi
export RTLIB_BTS_EVEN_POLY=1
export RT_BTS_CLEAR_IMAG=1
export OMP_NUM_THREADS=$NUM_CPUS
END=`expr $IMAGE_END '-' 1`
echo "[INFO] Test $TEST_NAME with images [$IMAGE_START, $END] by $NUM_CPUS threads."
./$TEST_NAME $DATA_FILE $IMAGE_START $END > $TEST_NAME.$ID.$IMAGE_START.$END.log 2>&1
[ $? -ne 0 ] && echo "[ERROR] failed to run $TEST_NAME. Check release_omp/$TEST_NAME.$ID.$IMAGE_START.$END.log for more details." && exit 1
[ ! -f $TEST_NAME.$ID.$IMAGE_START.$END.log ] && echo "[ERROR] failed to locate result file TEST_NAME.$ID.$IMAGE_START.$END.log" && exit 1

# check result
echo "[INFO] Test $TEST_NAME finished. Refer release_omp/$TEST_NAME.$ID.$IMAGE_START.$END.log for more details."
grep 'RESULT' $TEST_NAME.$ID.$IMAGE_START.$END.log

# go back to previous working dir
cd $WRK_DIR
