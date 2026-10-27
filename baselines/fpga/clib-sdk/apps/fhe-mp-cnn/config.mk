SDK_DIR ?= ../..
N = 65536
Q = 58
DATA_DIR = ./data
# Supported: MODEL=20 (CIFAR-10), MODEL=32 (CIFAR-100); SAMPLE=0,1,2.
MODEL ?= 20
SAMPLE ?= 0
export CNN_LAYERS := $(MODEL)
export CNN_DATASET := $(if $(filter 20,$(MODEL)),10,100)
export CNN_START := $(SAMPLE)
export CNN_END := $(SAMPLE)
WORK = work/resnet$(MODEL)/sample$(SAMPLE)
OUTPUT = result/resnet$(MODEL)/sample$(SAMPLE)
DATA_FILES := $(shell find data -type f)
