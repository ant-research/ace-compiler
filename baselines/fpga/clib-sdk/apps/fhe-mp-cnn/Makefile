include config.mk
.DEFAULT_GOAL := all
CNN_ROOT := src/cpu-ckks/single-key
CNN_SOURCES := $(wildcard src/common/*.cpp $(CNN_ROOT)/cnn/*.cpp $(CNN_ROOT)/comp/*.cpp) $(CNN_ROOT)/fhe_ops.cpp $(CNN_ROOT)/ckks_bootstrapping/Bootstrapper.cpp $(CNN_ROOT)/ckks_bootstrapping/ModularReducer.cpp
APP_OBJECTS := $(patsubst %.cpp,.build/%.o,$(CNN_SOURCES))
APP_HEADERS := $(shell find src -name '*.h') $(SDK_DIR)/include/seal/seal_api.h $(SDK_DIR)/include/seal/seal.h
APP_LIBS := $(SDK_DIR)/lib/libclib_seal.a
CPPFLAGS += -I$(SDK_DIR)/include -Isrc/common -I$(CNN_ROOT) -I$(CNN_ROOT)/cnn -I$(CNN_ROOT)/comp -I$(CNN_ROOT)/ckks_bootstrapping -DFHE_BACKEND_CPU -DCNN_DATA_ROOT='"$(abspath $(DATA_DIR))"'
CXXFLAGS := -O0 -std=c++17 -fopenmp -pthread $(CPPFLAGS)
LDLIBS += -lntl -lgmp -pthread
.build/src/%.o: src/%.cpp $(APP_HEADERS) config.mk Makefile
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@
include $(SDK_DIR)/tools/application.mk
all: report
.PHONY: report
report: verify
	OPENBLAS_NUM_THREADS=1 python3 tools/report.py --work "$(WORK)" --output "$(OUTPUT)" --model $(MODEL) --sample $(SAMPLE)

.PHONY: check-deps
build: check-deps
check-deps:
	@python3 -c "import numpy" || { echo "Reference checks require NumPy (python3 -m pip install numpy)."; exit 1; }
