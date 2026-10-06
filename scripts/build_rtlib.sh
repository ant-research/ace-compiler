#!/bin/bash -x
#
# Build the ACE rtlib examples (eg_fhertlib_* + the ant-dataset/imagenet
# apps) standalone, into ace-compiler/release_rtlib/ — without touching
# the main ace-compiler release/ build. Only builds (no install):
#   ace-compiler/release_rtlib/example/eg_fhertlib_*
#   ace-compiler/release_rtlib/dataset/...
#
# Why a separate script (not build_cmplr.sh - Dexample):
#   build_cmplr.sh configures the top-level fhe-cmplr project, which
#   pulls rtlib in as an ExternalProject. The rtlib subproject evaluates
#   ant/example/ against `if(FHE_BUILD_EXAMPLE)`, but that variable is
#   never passed to the rtlib subproject (only RTLIB_BUILD_EXAMPLE is),
#   so even with -DFHE_BUILD_EXAMPLE=ON at the top, rtlib's ant/example
#   stays guarded off. Building rtlib standalone lets us hand
#   `-DFHE_BUILD_EXAMPLE=ON` straight to the rtlib configure step.

working_dir=`pwd`
build_type=$1
# check build type
if [ -z "$1" ]; then
  build_type="Release"
fi
if [ "$build_type" != "Debug" -a "$build_type" != "Release" ]; then
  echo "Usage: $0 Debug|Release"
  exit 1
fi
build_dir=`echo $build_type | tr 'A-Z' 'a-z'`

ace_src="$working_dir/ace-compiler"
rtlib_src="$ace_src/fhe-cmplr/rtlib"
build_root="$ace_src/release_rtlib"

# Mirror the cmake args the top-level fhe-cmplr passes to the rtlib
# ExternalProject (see fhe-cmplr/cmake/modules/fhe_rtlib.cmake), plus
# -DFHE_BUILD_EXAMPLE=ON to satisfy ant/CMakeLists.txt's if(FHE_BUILD_EXAMPLE).
PACKAGE_BASE_DIR="${PACKAGE_BASE_DIR:-$ace_src/air-infra}"
PACKAGE_INC_DIR_TF="${PACKAGE_INC_DIR_TF:-$ace_src/air-infra/include|$ace_src/nn-addon/include|$ace_src/release/include|$ace_src/fhe-cmplr/include|$ace_src/fhe-cmplr/rtlib/include}"

cmake -S "$rtlib_src" -B "$build_root" \
  -G "Unix Makefiles" \
  -DCMAKE_BUILD_TYPE=$build_type \
  -DBUILD_STATIC=ON \
  -DBUILD_SHARED=OFF \
  -DBUILD_UNITTEST=OFF \
  -DBUILD_BENCH=OFF \
  -DRTLIB_BUILD_TEST=OFF \
  -DRTLIB_BUILD_EXAMPLE=ON \
  -DFHE_BUILD_EXAMPLE=ON \
  -DRTLIB_INSTALL_APP=OFF \
  -DRTLIB_ENABLE_SEAL=OFF \
  -DRTLIB_ENABLE_SEAL_BTS=OFF \
  -DRTLIB_ENABLE_OPENFHE=OFF \
  -DRTLIB_ENABLE_PHANTOM=OFF \
  -DRTLIB_ENABLE_ACE=OFF \
  -DRTLIB_SUPPORT_HPU=OFF \
  -DBUILD_WITH_OPENMP=OFF \
  -DRTLIB_ENABLE_LINUX=ON \
  -DRTLIB_SEED_MODE=prng \
  -DPACKAGE_BASE_DIR="$PACKAGE_BASE_DIR" \
  -DPACKAGE_INC_DIR_TF="$PACKAGE_INC_DIR_TF" \
  -DCMAKE_POLICY_VERSION_MINIMUM=3.5

if [ $? -ne 0 ]; then
  echo "Error: configure rtlib with CMake failed."
  exit 1
fi

cmake --build "$build_root" -j 16
if [ $? -ne 0 ]; then
  echo "Error: build rtlib with CMake failed."
  exit 2
fi

echo "Info: rtlib examples built. eg_fhertlib_* at $build_root/example/"
exit 0
