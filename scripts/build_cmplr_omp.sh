#!/bin/bash

working_dir=`pwd`

# check build type
if [ "$1" != "Debug" -a "$1" != "Release" ]; then
  echo "Usage: $0 Debug|Release"
  exit 0
fi
build_type=$1
build_dir=`echo $1 | tr 'A-Z' 'a-z'`_openmp

# check directory
check_source_directory() {
  if [ ! -d ./$1 ]; then
    echo "Error: not find ./$1. make sure running this script in the same directory with $1"
    exit 0
  fi
}
check_source_directory air-infra
check_source_directory nn-addon
check_source_directory fhe-cmplr

# configure compiler with cmake
cmake -S fhe-cmplr -B $build_dir -DFHE_WITH_SRC="air-infra;nn-addon" -DBUILD_UNITTEST=OFF -DAIR_BUILD_EXAMPLE=OFF -DBUILD_BENCH=OFF -DCMAKE_BUILD_TYPE=$build_type -DAIR_CODE_CHECK=OFF -DNN_CODE_CHECK=OFF -DFHE_CODE_CHECK=OFF -DBUILD_WITH_OPENMP=ON
if [ $? -ne 0 ]; then
  echo "Error: configure project with CMake failed."
  exit 1
fi

cmake --build $build_dir -j
if [ $? -ne 0 ]; then
  echo "Error: build project with CMake failed."
  exit 2
fi

echo "Info: build project succeeded. FHE compiler executable can be found in $build_dir/driver/fhe_cmplr"
exit 0
