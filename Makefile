all: bin/buddhabrot bin/omp bin/evercat bin/raw2bmp 

clean:
	rm *.x

CFLAGS=-Wall -Wextra
MAKE_BIN_DIR=mkdir -p bin

# Original reference version by Evercat
bin/evercat: original.c
	$(MAKE_BIN_DIR)
	g++ -O2 -o $@ $<

# Single Core
bin/buddhabrot: buddhabrot.cpp
	$(MAKE_BIN_DIR)
	g++ -O2 $(CFLAGS) $< -o $@

# Multi Core (OpenMP)
LFLAGS=-fopenmp
bin/omp: buddhabrot_omp.cpp
	$(MAKE_BIN_DIR)
	g++ -O2 $(CFLAGS) $< -o $@ $(LFLAGS)

# CUDA
bin/cuda: buddhrabrot_cuda.cu
	$(MAKE_BIN_DIR)
	nvcc $(CFLAGS) $< -o $@

# Utility
bin/raw2bmp: raw2bmp.cpp
	$(MAKE_BIN_DIR)
	g++ -O2 $(CFLAGS) $< -o $@

