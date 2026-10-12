# SDK-owned build rules. Application settings belong in config.mk.
include config.mk
include $(SDK_DIR)/tools/application.mk

.PHONY: prediction
all: prediction
prediction: verify
	@.build/app --report
