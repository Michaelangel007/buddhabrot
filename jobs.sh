#!/bin/bash

mkdir -p jobs
cd       jobs

../bin/omp4 -j1 -raw 1.data -bmp 1.bmp ; echo ""
../bin/omp4 -j2 -raw 2.data -bmp 2.bmp ; echo ""
../bin/omp4 -j3 -raw 3.data -bmp 3.bmp ; echo ""
../bin/omp4 -j4 -raw 4.data -bmp 4.bmp ; echo ""
../bin/omp4 -j5 -raw 5.data -bmp 5.bmp ; echo ""
../bin/omp4 -j6 -raw 6.data -bmp 6.bmp ; echo ""
../bin/omp4 -j7 -raw 7.data -bmp 7.bmp ; echo ""
../bin/omp4 -j8 -raw 8.data -bmp 8.bmp ; echo ""

echo "Comparing raw images..."
diff 1.data 2.data
diff 1.data 3.data
diff 1.data 4.data

cd ..

