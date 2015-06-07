#!/bin/bash

mkdir -p data
cd data
../bin/buddhabrot

../bin/omp

diff "cpu1_raw_buddhabrot_1024x768_1000_10x.u16.data" "omp1_raw_buddhabrot_1024x768_1000_10x.u16.data"

../bin/omp2

diff "cpu1_raw_buddhabrot_1024x768_1000_10x.u16.data" "omp2_raw_buddhabrot_1024x768_1000_10x.u16.data"

