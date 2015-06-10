#!/bin/bash

mkdir -p verify
cd       verify

echo "\nSingle-threaded ...      " ; ../bin/buddhabrot
echo "\nMulti-threaded v1 ...    " ; ../bin/omp1
echo "\nMulti-threaded v2 ...    " ; ../bin/omp2
echo "\nMulti-threaded v3 ...    " ; ../bin/omp3
echo "\nMulti-threaded v3 float32" ; ../bin/omp3float32
echo "\nMulti-threaded v4 ...    " ; ../bin/omp4

echo -e "\nComparing raw images ..."

diff "raw_cpu1_buddhabrot_1024x768_1000_10x.u16.data" "raw_omp1_buddhabrot_1024x768_1000_10x.u16.data"
diff "raw_cpu1_buddhabrot_1024x768_1000_10x.u16.data" "raw_omp2_buddhabrot_1024x768_1000_10x.u16.data"
diff "raw_cpu1_buddhabrot_1024x768_1000_10x.u16.data" "raw_omp3_buddhabrot_1024x768_1000_10x.u16.data"
diff "raw_cpu1_buddhabrot_1024x768_1000_10x.u16.data" "raw_omp4_buddhabrot_1024x768_1000_10x.u16.data"

echo "Compared float64 with float32"
diff "raw_omp3_buddhabrot_1024x768_1000_10x.u16.data" "raw_omp3float_buddhabrot_1024x768_1000_10x.u16.data"

echo "Compared two Fastest verions v3 and v4"
diff "raw_omp3_buddhabrot_1024x768_1000_10x.u16.data" "raw_omp4_buddhabrot_1024x768_1000_10x.u16.data"

