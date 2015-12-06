# https://github.com/Michaelangel007/buddhabrot

all: bin/buddhabrot                                            \
     bin/omp1 bin/omp2 bin/omp3 bin/omp3float bin/omp4         \
     bin/evercat bin/raw2bmp bin/mandelbrot bin/mandelbrot_omp \
     bin/text_mandelbrot bin/text_buddhabrot                   \
     bin/c11

omp: bin/omp1
cuda: bin/cuda bin/cuda_info

# References:
# http://stackoverflow.com/questions/714100/os-detecting-makefile
# http://stackoverflow.com/questions/6543364/how-do-i-code-a-makefile-that-can-tell-the-difference-between-intel-os-x-and-lin
# http://stackoverflow.com/questions/4058840/makefile-that-distincts-between-windows-and-unix-like-systems
# http://git.kernel.org/cgit/git/git.git/tree/Makefile
# http://stackoverflow.com/questions/3707517/make-file-echo-displaying-path-string
# http://stackoverflow.com/questions/6038040/printing-variable-from-within-makefile

ifeq ($(OS),Windows_NT)
  $(info "Make OS: Windows detected...");
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
    $(info Make OS: Linux detected...)
    CC     := g++
    CFLAGS += -DOS_LINUX
    CORES  := $(shell grep -c ^processor /proc/cpuinfo)
    $(info Make cores: $(CORES))
  endif
  ifeq ($(OS),Darwin) # OSX
    $(info Make OS: OSX detected...)
    CFLAGS += -DOS_OSX
    # LLVM resed in /usr/bin/g++
    # custom GCC in /usr/local/bin/g++
    CC := /usr/local/bin/g++
  endif
endif


MAKE_BIN_DIR=mkdir -p bin

LIB_OMP=-fopenmp
LIB_C11=-std=c++11 -lstdc++

# You may need to link with the the Standard C++ libary
#    -lstdc++
LFLAGS=

CUDAFLAGS=-O2

clean:
	$(RM) bin/*

# Tutorial Mandelbrot
bin/text_mandelbrot: text_mandelbrot.cpp
	@$(MAKE_BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $<

# Tutorial Buddhabrot
bin/text_buddhabrot: text_buddhabrot.cpp
	@$(MAKE_BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $<

# Original reference version by Evercat
# It doesn't include any built-in timing/benchmark
# so you will have to time manually:
#    time bin/evercat
bin/evercat: original.c
	@$(MAKE_BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $<

# Single Core
bin/buddhabrot: buddhabrot.cpp
	@$(MAKE_BIN_DIR)
	$(CC) $(CFLAGS) $< -o $@

# Multi Core (OpenMP) Faster - First version - parallel outer loop
bin/omp1: buddhabrot_omp1.cpp
	@$(MAKE_BIN_DIR)
	$(CC) $(CFLAGS) $< -o $@ $(LIB_OMP)

# Multi Core (OpenMP) Faster - Second version - parallel outer and inner loop -> linearized
bin/omp2: buddhabrot_omp2.cpp
	@$(MAKE_BIN_DIR)
	$(CC) $(CFLAGS) $< -o $@ $(LIB_OMP)

# Multi Core (OpenMP) Faster 2 - Third version - parallel outer and inner loop -> linearized
bin/omp3: buddhabrot_omp3.cpp
	@$(MAKE_BIN_DIR)
	$(CC) $(CFLAGS) $< -o $@ $(LIB_OMP)

# Multi Core (OpenMP) Fastest - Fourth version - optimized plot()
bin/omp4: buddhabrot_omp4.cpp
	@$(MAKE_BIN_DIR)
	$(CC) $(CFLAGS) $< -o $@ $(LIB_OMP)

# C++11
bin/c11: buddhabrot_c11.cpp
	@$(MAKE_BIN_DIR)
	$(CC) $(CFLAGS) $< -o $@ $(LIB_C11)

# Multi Core (OpenMP) Float
bin/omp3float: buddhabrot_omp3float.cpp
	@$(MAKE_BIN_DIR)
	$(CC) $(CFLAGS) $< -o $@ $(LIB_OMP)

# Utility
bin/raw2bmp: raw2bmp.cpp
	@$(MAKE_BIN_DIR)
	$(CC) $(CFLAGS) $< -o $@

# CUDA
bin/cuda: buddhrabrot_cuda.cu
	@$(MAKE_BIN_DIR)
	nvcc $(CUDAFLAGS) $< -o $@

# CUDA Info
bin/cuda_info: cuda_info.cu
	@$(MAKE_BIN_DIR)
	nvcc $(CUDAFLAGS) $< -o $@

# Single Core Mandelbrot
bin/mandelbrot: mandelbrot.cpp
	@$(MAKE_BIN_DIR)
	$(CC) $(CFLAGS) $< -o $@

# Multi Core (OpenMP) Mandelbrot
bin/mandelbrot_omp: mandelbrot.cpp
	@$(MAKE_BIN_DIR)
	$(CC) $(CFLAGS) -DOMP $< -o $@ $(LIB_OMP)

