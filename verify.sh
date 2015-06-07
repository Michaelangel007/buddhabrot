#!/bin/bash

mkdir -p data
cd data

echo -e "\nSingle core...   " ; ../bin/buddhabrot
echo -e "\nMulti core v1 ..." ; ../bin/omp1
echo -e "\nMulti core v2 ..." ; ../bin/omp2

echo -e "\nComparing raw bitmaps ..."

diff "raw_cpu1_buddhabrot_1024x768_1000_10x.u16.data" "raw_omp1_buddhabrot_1024x768_1000_10x.u16.data"
diff "raw_cpu1_buddhabrot_1024x768_1000_10x.u16.data" "raw_omp2_buddhabrot_1024x768_1000_10x.u16.data"

