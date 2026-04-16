# Makefile for voice_in
#
# Usage:
#     make setup      # clone whisper.cpp, build it, download the model
#     make            # build the voice_in binary
#     make run        # launch it
#     make clean      # remove voice_in binary
#     make distclean  # also remove whisper.cpp
#
# CUDA is auto-detected. If nvcc and libcudart are found, whisper.cpp is
# built with GPU support. Otherwise everything runs on CPU.
#
# System deps (apt):
#     build-essential cmake pkg-config git
#     libgtk-3-dev libnotify-dev libportaudio-dev libcairo2-dev
#     xclip libnotify-bin
#     (optional) nvidia-cuda-toolkit or equivalent providing nvcc

CC       := gcc
CFLAGS   := -O2 -Wall -Wextra -Wno-unused-parameter -Wno-deprecated-declarations -std=c11
LDFLAGS  :=

WHISPER_DIR   := whisper.cpp
WHISPER_BUILD := $(WHISPER_DIR)/build

MODEL_NAME := medium
MODEL_FILE := $(WHISPER_DIR)/models/ggml-$(MODEL_NAME).bin

PKG_CFLAGS := $(shell pkg-config --cflags gtk+-3.0 libnotify portaudio-2.0)
PKG_LIBS   := $(shell pkg-config --libs   gtk+-3.0 libnotify portaudio-2.0)

INCLUDES     := -I$(WHISPER_DIR)/include -I$(WHISPER_DIR)/ggml/include
WHISPER_LIBS := $(shell find $(WHISPER_BUILD) -name 'lib*.a' 2>/dev/null)

# --- CUDA auto-detection ---
NVCC_FOUND   := $(shell command -v nvcc 2>/dev/null)
CUDART_FOUND := $(wildcard /usr/local/cuda/lib64/libcudart.so)

ifneq ($(NVCC_FOUND),)
ifneq ($(CUDART_FOUND),)
    HAS_CUDA     := 1
    CUDA_DIR     := /usr/local/cuda
    CUDA_LIBS    := -L$(CUDA_DIR)/lib64 -lcudart -lcublas -lcublasLt -lcuda
    CUDA_CMAKE   := -DGGML_CUDA=ON
else
    HAS_CUDA     := 0
    CUDA_LIBS    :=
    CUDA_CMAKE   :=
endif
else
    HAS_CUDA     := 0
    CUDA_LIBS    :=
    CUDA_CMAKE   :=
endif

# ---- targets ----

all: voice_in

voice_in: voice_in.c
ifeq ($(HAS_CUDA),1)
	@echo "[build] CUDA detected — linking with GPU support"
else
	@echo "[build] CUDA not found — CPU only"
endif
	$(CC) $(CFLAGS) $(INCLUDES) $(PKG_CFLAGS) \
	    voice_in.c -o $@ \
	    $(WHISPER_LIBS) \
	    $(PKG_LIBS) $(CUDA_LIBS) $(LDFLAGS) \
	    -lm -lpthread -lstdc++ -fopenmp

setup:
	@if [ ! -d "$(WHISPER_DIR)" ]; then \
	    git clone --depth 1 https://github.com/ggerganov/whisper.cpp.git $(WHISPER_DIR); \
	fi
ifeq ($(HAS_CUDA),1)
	@echo "[setup] CUDA detected — building whisper.cpp with GPU support"
else
	@echo "[setup] CUDA not found — building whisper.cpp for CPU only"
endif
	cd $(WHISPER_DIR) && cmake -B build \
	    -DCMAKE_BUILD_TYPE=Release \
	    -DBUILD_SHARED_LIBS=OFF \
	    -DWHISPER_BUILD_EXAMPLES=OFF \
	    -DWHISPER_BUILD_TESTS=OFF \
	    $(CUDA_CMAKE)
	cmake --build $(WHISPER_BUILD) -j$$(nproc) --target whisper
	@if [ ! -f "$(MODEL_FILE)" ]; then \
	    cd $(WHISPER_DIR) && bash ./models/download-ggml-model.sh $(MODEL_NAME); \
	fi
	@echo ""
	@echo "setup done. Run: make && make run"

run: voice_in
	VOICE_IN_MODEL=$(MODEL_FILE) ./voice_in

clean:
	rm -f voice_in

distclean: clean
	rm -rf $(WHISPER_DIR)

.PHONY: all setup run clean distclean
