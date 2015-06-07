#!/bin/bash

mkdir -p data
cd data

echo -e "\n" ; ../bin/buddhabrot
echo -e "\n" ; ../bin/omp1
echo -e "\n" ; ../bin/omp2

echo "Comparing ..."

diff "cpu1_buddhabrot_1024x768_1000_10x.u16.data" "omp1_buddhabrot_1024x768_1000_10x.u16.data"
diff "cpu1_buddhabrot_1024x768_1000_10x.u16.data" "omp2_buddhabrot_1024x768_1000_10x.u16.data"

