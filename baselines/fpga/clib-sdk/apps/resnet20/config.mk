SDK_DIR ?= ../..
N = 1024
Q = 62
DATA_DIR = ./data
DATA_FILES = $(wildcard $(DATA_DIR)/conv*.weight $(DATA_DIR)/conv*.bias $(DATA_DIR)/sample0.*)
