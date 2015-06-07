# https://github.com/Michaelangel007/buddhabrot

all: bin/buddhabrot bin/omp bin/omp2 bin/evercat bin/raw2bmp

omp: bin/omp
cuda: bin/cuda

# References:
# http://stackoverflow.com/questions/714100/os-detecting-makefile
# http://stackoverflow.com/questions/6543364/how-do-i-code-a-makefile-that-can-tell-the-difference-between-intel-os-x-and-lin
# http://stackoverflow.com/questions/4058840/makefile-that-distincts-between-windows-and-unix-like-systems
# http://git.kernel.org/cgit/git/git.git/tree/Makefile
# http://stackoverflow.com/questions/3707517/make-file-echo-displaying-path-string
# http://stackoverflow.com/questions/6038040/printing-variable-from-within-makefile

ifeq ($(OS),Windows_NT)
  $(info "OS: Windows detected...");
  CFLAGS += -DOS_WIN
  RM := del /q
else
  # We do NOT use any switches since we want the short OS name
  #   -a   Display full info
  CFLAGS += -Wall -Wextra -O2
  OS := $(shell uname)
  RM := rm
  ifeq ($(OS),Linux) # Various distributions
    # Using $(info ...text...) since @echo doesn't work
    $(info OS: Linux detected...)
    CC     := g++
    CFLAGS += -DOS_LINUX
    CORES  := $(shell grep -c ^processor /proc/cpuinfo)
    $(info Cores: $(CORES))
  endif
  ifeq ($(OS),Darwin) # OSX
    $(info "OS: OSX detected...")
    CFLAGS += -DOS_OSX
    # LLVM resed in /usr/bin/g++
    # custom GCC in /usr/local/bin/g++
    CC := /usr/local/bin/g++
  endif
endif


MAKE_BIN_DIR=mkdir -p bin
# You may need to link with the the Standard C++ libary
#    -lstdc++
LFLAGS=-fopenmp


clean:
	$(RM) bin/*

# Original reference version by Evercat
# It doesn't include any built-in timing/benchmark
# so you will have to time manually:
#    time bin/evercat
bin/evercat: original.c
	$(MAKE_BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $<

# Single Core
bin/buddhabrot: buddhabrot.cpp
	$(MAKE_BIN_DIR)
	$(CC) $(CFLAGS) $< -o $@

# Multi Core (OpenMP) First version - parallel outer loop
bin/omp: buddhabrot_omp.cpp
	$(MAKE_BIN_DIR)
	$(CC) $(CFLAGS) $< -o $@ $(LFLAGS)

# Multi Core (OpenMP) Second version - parallel outer and inner loop
bin/omp2: buddhabrot_omp2.cpp
	$(MAKE_BIN_DIR)
	$(CC) $(CFLAGS) $< -o $@ $(LFLAGS)

# CUDA
bin/cuda: buddhrabrot_cuda.cu
	$(MAKE_BIN_DIR)
	nvcc $(CFLAGS) $< -o $@

# Utility
bin/raw2bmp: raw2bmp.cpp
	$(MAKE_BIN_DIR)
	$(CC) $(CFLAGS) $< -o $@

