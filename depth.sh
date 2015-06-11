#!/bin/bash

mkdir -p depth
cd       depth

echo -e "\nDepth 4000"
../bin/omp3 -v 4000 3000 4000
../bin/omp3    4000 3000 4000

echo -e "\nDepth 3000"
../bin/omp3 -v 4000 3000 3000
../bin/omp3    4000 3000 3000

echo -e "\nDepth 2000"
../bin/omp3 -v 4000 3000 2000
../bin/omp3    4000 3000 2000

echo -e "\nDepth 1000"
../bin/omp3 -v 4000 3000 1000
../bin/omp3    4000 3000 1000

