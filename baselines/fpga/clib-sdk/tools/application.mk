.DEFAULT_GOAL := all
CXX ?= g++
CXXFLAGS ?= -O2 -std=c++17
SDK_DIR := $(abspath $(SDK_DIR))
DATA_DIR ?= .
APP_SOURCES ?= main.cpp
APP_OBJECTS ?=
APP_LIBS ?=
APP_HEADERS ?=
APP := .build/app
WORK ?= work
OUTPUT ?= result
TOOL := $(SDK_DIR)/tools/clib-ant-tool
.NOTPARALLEL:
.PHONY: all build export context encode run verify clean distclean
all: verify
build: $(APP)
$(APP): $(APP_SOURCES) $(APP_OBJECTS) $(APP_HEADERS) $(APP_LIBS) config.mk $(SDK_DIR)/include/clib.h $(SDK_DIR)/lib/libclib.a
	@mkdir -p .build
	$(CXX) $(CXXFLAGS) -DCLIB_APP_DATA_DIR='"$(abspath $(DATA_DIR))"' -I"$(SDK_DIR)/include" $(APP_SOURCES) $(APP_OBJECTS) $(APP_LIBS) "$(SDK_DIR)/lib/libclib.a" $(LDLIBS) -fopenmp -lm -o "$@"
export: build $(DATA_FILES)
	@mkdir -p "$(OUTPUT)"
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
clean:
	rm -rf -- .build
# Explicitly removes generated keys, encoded data and results.
distclean: clean
	rm -rf -- "$(WORK)" "$(OUTPUT)"
