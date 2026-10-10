# fhe-mp-cnn: FHE-MP-CNN homomorphic ResNet (baselines/fpga/cnn_ckks,
# clib-adapted) on the real clib backend through the SEAL compatibility
# layer (baselines/fpga/clib-seal). Besides the standard app settings this
# file carries the build glue that turns the single-file SDK application
# build into a whole-network one: tools/application.mk compiles main.cpp
# and links libclib.a, so the staged CNN sources and the shim are compiled
# by the rules below and fed to the same link line through CXXFLAGS.

SDK_DIR ?= ../..

# FHE-MP-CNN profile: N=65536 (32768 slots) and clib's maximum Q of 62
# (the staged infer_seal.cpp trims the chain accordingly).
N = 65536
Q = 62

# Model / image selection, exported to the application binary, which reads
# CNN_LAYERS / CNN_DATASET / CNN_START / CNN_END (see main.cpp). Supported
# pairings follow the pretrained data actually present under DATA_DIR:
# ResNet-20 on CIFAR-10 and ResNet-32 on CIFAR-100.
export CNN_LAYERS ?= 20
export CNN_DATASET ?= 10
export CNN_START ?= 0
export CNN_END ?= 0

# Raw data root: the FHE-MP-CNN weights and CIFAR test images shipped with
# the gpu route (**/baselines/gpu; the fpga route carries no local copy).
# The CNN reads $(DATA_DIR)/pretrained_parameters/<model>/... and
# $(DATA_DIR)/testFile[_cifar100]/... (passed to the build as -DCNN_DATA_ROOT,
# the same role as CLIB_APP_DATA_DIR in the other apps); its own report
# files go to this app's result/ (-DCNN_RESULT_DIR).
DATA_DIR = ../../../../gpu

# Files main.cpp (via the staged CNN) must be able to open. Wildcards
# expand to nothing when absent, so one direct reference per dataset is
# kept to make Make fail early on missing data. d13.txt holds the ReLU
# minimax composition coefficients (upstream FHE-MP-CNN artifact); without
# it the CNN silently degenerates to a ReLU-less classifier.
ifeq ($(CNN_LAYERS)$(CNN_DATASET),2010)
DATA_FILES := $(wildcard $(DATA_DIR)/pretrained_parameters/resnet20_new/*.txt) \
              $(DATA_DIR)/testFile/test_values.txt \
              $(DATA_DIR)/testFile/test_label.txt \
              $(DATA_DIR)/result/d13.txt
else ifeq ($(CNN_LAYERS)$(CNN_DATASET),32100)
DATA_FILES := $(wildcard $(DATA_DIR)/pretrained_parameters/resnet32_new/*.txt) \
              $(DATA_DIR)/testFile_cifar100/test_values.txt \
              $(DATA_DIR)/testFile_cifar100/test_label.txt \
              $(DATA_DIR)/result/d13.txt
else
# Other CNN_LAYERS/CNN_DATASET combinations have no staged data; the
# application rejects them at import time.
DATA_FILES :=
endif

# ---- build glue: staged CNN sources + SEAL compatibility layer -----------

FPGA_ROOT := $(abspath ../../..)
REPO_ROOT := $(abspath ../../../../..)
CLIB_SEAL := $(FPGA_ROOT)/clib-seal
STAGE_SRC := $(FPGA_ROOT)/build/cnn-ckks-src
STAGE_TOOL := $(REPO_ROOT)/scripts/build_cnn_clib.py
STAGE_STAMP := $(STAGE_SRC)/.clib-staged
NTL_INC := $(dir $(firstword $(wildcard /usr/local/include/NTL/RR.h /usr/include/NTL/RR.h)))

# The clib-adapted CNN tree (two-prime level chain, per-cycle second
# bootstrap, save_output, clib-backed Bootstrapper, data-root redirects -
# every SDK-backend adaptation) is staged by scripts/build_cnn_clib.py
# from the pristine cnn_ckks tree. The stamp's prerequisites are the
# patch inputs, so make re-stages exactly when they change.
$(STAGE_STAMP): $(STAGE_TOOL) \
                $(REPO_ROOT)/scripts/build_clib_sdk.py \
                $(FPGA_ROOT)/cnn_ckks/cpu-ckks/single-key/cnn/infer_seal.cpp \
                $(FPGA_ROOT)/cnn_ckks/cpu-ckks/single-key/cnn/infer_seal.h \
                $(CLIB_SEAL)/seal/seal.h \
                $(CLIB_SEAL)/seal_api.h \
                $(CLIB_SEAL)/seal_api.cxx \
                $(CLIB_SEAL)/Bootstrapper.h \
                $(CLIB_SEAL)/Bootstrapper.cpp
	@echo "[fhe-mp-cnn] staging clib-adapted CNN sources"
	python3 "$(STAGE_TOOL)" --stage-only
	@touch "$@"

# Translation units on top of main.cpp (the cnn target's source list in
# cnn_ckks/CMakeLists.txt): the staged CNN plus the shim itself.
CNN_SOURCES := \
  $(addprefix $(STAGE_SRC)/common/,Choosemax.cpp MinicompFunc.cpp MinicompRemez.cpp Point.cpp PolyUpdate.cpp func.cpp Polynomial.cpp Remez.cpp RemezApp.cpp) \
  $(addprefix $(STAGE_SRC)/cpu-ckks/single-key/cnn/,cnn_seal.cpp infer_seal.cpp) \
  $(STAGE_SRC)/cpu-ckks/single-key/fhe_ops.cpp \
  $(addprefix $(STAGE_SRC)/cpu-ckks/single-key/comp/,program.cpp SEALcomp.cpp SEALfunc.cpp) \
  $(addprefix $(STAGE_SRC)/cpu-ckks/single-key/ckks_bootstrapping/,Bootstrapper.cpp ModularReducer.cpp)

# The shim must be built exactly as the cmake route builds it
# (SEALConfig.cmake compiles seal_clib_shim inside the consumer's Release
# configuration with no target_compile_options of its own): its
# scale-correction helpers run through subnormal-heavy double chains whose
# last-bit rounding is optimization-sensitive, and the export's object names
# are keyed on those values. Any flag drift (-O level, -march=native, even
# -std=c++17 vs gnu++17) changes the declaration set (2346 objects for the
# canonical flags vs 2304 at -O0, same 258 rotations/24706 operations) and
# decouples this app's materials from the scripts/run_cnn_clib.py flow.
# Flags mirror cmake: Release (-O3 -DNDEBUG), cxx_std_17 -> -std=gnu++17,
# OpenMP/PIC, and the package include dirs - nothing else.
SHIM_CXXFLAGS := -O3 -DNDEBUG -std=gnu++17 -fopenmp -pthread -fPIC \
  -I"$(CLIB_SEAL)" -I"$(SDK_DIR)/include"
.build/seal_api.o: $(CLIB_SEAL)/seal_api.cxx $(STAGE_STAMP)
	@mkdir -p .build
	$(CXX) $(SHIM_CXXFLAGS) -c -o $@ $<

# Compile flags matching the cmake-built cnn target (-O0 -march=native, as
# validated; see scripts/build_cnn_clib.py) plus the data-root macros the
# staged sources read (see the CNN_DATA_ROOT/CNN_RESULT_DIR preamble the
# staging script injects into infer_seal.cpp).
CNN_DATA_FLAGS := -DCNN_DATA_ROOT='"$(abspath $(DATA_DIR))"' -DCNN_RESULT_DIR='"result"'
CNN_CXXFLAGS := -w -O0 -std=c++17 -pthread -march=native -fopenmp -DFHE_BACKEND_CPU \
  -I"$(CLIB_SEAL)" -I"$(SDK_DIR)/include" -I$(NTL_INC) \
  -I"$(STAGE_SRC)/common" \
  -I"$(STAGE_SRC)/cpu-ckks/single-key/cnn" \
  -I"$(STAGE_SRC)/cpu-ckks/single-key/comp" \
  -I"$(STAGE_SRC)/cpu-ckks/single-key/ckks_bootstrapping" \
  -I"$(STAGE_SRC)/cpu-ckks/single-key" \
  $(CNN_DATA_FLAGS)

CNN_OBJS := $(foreach s,$(CNN_SOURCES),.build/$(notdir $(basename $(s))).o)
SEAL_OBJ := .build/seal_api.o

define cnn_compile
.build/$(notdir $(basename $(1))).o: $(1) $(STAGE_STAMP)
	@mkdir -p .build
	$$(CXX) $$(CNN_CXXFLAGS) -c -o $$@ $$<
endef
$(foreach s,$(CNN_SOURCES),$(eval $(call cnn_compile,$(s))))

# application.mk links main.cpp with libclib.a only, so hook the staged
# objects into its .build/app link line via CXXFLAGS (object files link
# regardless of their position on the command line) and make the app depend
# on them; -lntl/-lgmp serve the staged CNN's Remez/bootstrapping code.
CXXFLAGS := -w -O0 -std=c++17 -pthread -march=native -fopenmp \
  -I"$(CLIB_SEAL)" $(CNN_DATA_FLAGS) \
  $(CNN_OBJS) $(SEAL_OBJ) -lntl -lgmp -lpthread
.build/app: $(CNN_OBJS) $(SEAL_OBJ)