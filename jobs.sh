#!/bin/bash

mkdir -p jobs
cd       jobs

../bin/omp4 -j1 ; echo "" ; mv raw_omp4_buddhabrot_1024x768_1000_10x.u16.data 1.data ; mv omp4_buddhabrot_768x1024_1000.bmp 1.bmp
../bin/omp4 -j2 ; echo "" ; mv raw_omp4_buddhabrot_1024x768_1000_10x.u16.data 2.data ; mv omp4_buddhabrot_768x1024_1000.bmp 2.bmp
../bin/omp4 -j3 ; echo "" ; mv raw_omp4_buddhabrot_1024x768_1000_10x.u16.data 3.data ; mv omp4_buddhabrot_768x1024_1000.bmp 3.bmp
../bin/omp4 -j4 ; echo "" ; mv raw_omp4_buddhabrot_1024x768_1000_10x.u16.data 4.data ; mv omp4_buddhabrot_768x1024_1000.bmp 4.bmp
../bin/omp4 -j5 ; echo "" ; mv raw_omp4_buddhabrot_1024x768_1000_10x.u16.data 5.data ; mv omp4_buddhabrot_768x1024_1000.bmp 5.bmp
../bin/omp4 -j6 ; echo "" ; mv raw_omp4_buddhabrot_1024x768_1000_10x.u16.data 6.data ; mv omp4_buddhabrot_768x1024_1000.bmp 6.bmp
../bin/omp4 -j7 ; echo "" ; mv raw_omp4_buddhabrot_1024x768_1000_10x.u16.data 7.data ; mv omp4_buddhabrot_768x1024_1000.bmp 7.bmp
../bin/omp4 -j8 ; echo "" ; mv raw_omp4_buddhabrot_1024x768_1000_10x.u16.data 8.data ; mv omp4_buddhabrot_768x1024_1000.bmp 8.bmp

echo "Comparing raw images..."
diff 1.data 2.data
diff 1.data 3.data
diff 1.data 4.data

cd ..

