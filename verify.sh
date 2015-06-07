#!/bin/bash

mkdir -p data
cd data

../bin/buddhabrot
../bin/omp1
../bin/omp2

diff "cpu1_buddhabrot_1024x768_1000_10x.u16.data" "omp1_buddhabrot_1024x768_1000_10x.u16.data"
diff "cpu1_buddhabrot_1024x768_1000_10x.u16.data" "omp2_buddhabrot_1024x768_1000_10x.u16.data"

