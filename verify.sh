#!/bin/bash

mkdir -p verify
cd       verify

echo -e "\nSingle-threaded ...      " ; ../bin/buddhabrot -raw cpu1.data
echo -e "\nMulti-threaded v1 ...    " ; ../bin/omp1       -raw omp1.data
echo -e "\nMulti-threaded v2 ...    " ; ../bin/omp2       -raw omp2.data
echo -e "\nMulti-threaded v3 ...    " ; ../bin/omp3       -raw omp3.data
echo -e "\nMulti-threaded v3 float32" ; ../bin/omp3float  -raw omp3float.data
echo -e "\nMulti-threaded v4 ...    " ; ../bin/omp4       -raw omp4.data

echo -e "\nComparing raw images ..."

diff "cpu1.data" "omp1.data"
diff "cpu1.data" "omp2.data"
diff "cpu1.data" "omp3.data"
diff "cpu1.data" "omp4.data"

echo -e "\nCompare float64 with float32"
diff "omp3.data" "omp3float.data"

echo -e "\bCompare two Fastest verions v3 and v4"
diff "omp3.data" "omp4.data"

