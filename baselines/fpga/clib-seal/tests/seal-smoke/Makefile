# SEAL-compatible-layer smoke test (see main.cpp). Mirrors the SDK's
# apps/*/Makefile five-step flow (export -> context -> encode -> run ->
# verify), but compiles main.cpp together with the shim (seal_api.cxx) and
# the SEAL-compatible package headers.
SDK_DIR ?= /app/baselines/fpga/clib-sdk
SEAL_PKG ?= /app/baselines/fpga/build/clib-seal-pkg

CXX ?= g++
CXXFLAGS ?= -O2 -std=c++17

APP := .build/app
WORK := work
OUTPUT := result
TOOL := $(SDK_DIR)/tools/clib-ant-tool
N = 1024
Q = 38

.NOTPARALLEL:
.PHONY: all build export context encode run verify clean distclean

all: verify

$(APP): main.cpp $(SEAL_PKG)/src/seal_api.cxx $(SEAL_PKG)/include/seal/seal.h
	@mkdir -p .build
	$(CXX) $(CXXFLAGS) -I"$(SEAL_PKG)/include" -I"$(SDK_DIR)/include" main.cpp \
		"$(SEAL_PKG)/src/seal_api.cxx" $(SDK_DIR)/lib/libclib.a -fopenmp -lm -o "$@"

export: build
	@echo "[1/5] Export application requirements"
	@"$(APP)" --clib-mode export --N "$(N)" --Q "$(Q)" --work "$(WORK)" --output "$(OUTPUT)"

context: export
	@echo "[2/5] Prepare/reuse ANT context"
	@"$(TOOL)" application-context --work "$(WORK)"

encode: context
	@echo "[3/5] Encode/encrypt application objects with ANT"
	@"$(TOOL)" application-encode --work "$(WORK)"

run: encode
	@echo "[4/5] Execute clib graph"
	@OMP_WAIT_POLICY=PASSIVE "$(APP)" --clib-mode run --N "$(N)" --Q "$(Q)" --work "$(WORK)" --output "$(OUTPUT)"

verify: run
	@echo "[5/5] Decrypt and verify"
	@"$(TOOL)" application-verify --work "$(WORK)" --output "$(OUTPUT)"

build: $(APP)

clean:
	rm -rf -- .build

distclean: clean
	rm -rf -- "$(WORK)" "$(OUTPUT)"