#!/bin/bash

mkdir -p jobs
cd       jobs

../bin/omp3 -j1 ; echo "" ; mv raw_omp3_buddhabrot_1024x768_1000_10x.u16.data 1.data ; mv omp3_buddhabrot_768x1024_1000.bmp 1.bmp
../bin/omp3 -j2 ; echo "" ; mv raw_omp3_buddhabrot_1024x768_1000_10x.u16.data 2.data ; mv omp3_buddhabrot_768x1024_1000.bmp 2.bmp
../bin/omp3 -j3 ; echo "" ; mv raw_omp3_buddhabrot_1024x768_1000_10x.u16.data 3.data ; mv omp3_buddhabrot_768x1024_1000.bmp 3.bmp
../bin/omp3 -j4 ; echo "" ; mv raw_omp3_buddhabrot_1024x768_1000_10x.u16.data 4.data ; mv omp3_buddhabrot_768x1024_1000.bmp 4.bmp

echo "Comparing raw images..."
diff 1.data 2.data
diff 1.data 3.data
diff 1.data 4.data

cd ..

