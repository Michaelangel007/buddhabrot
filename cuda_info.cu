/*
    nvcc cuda_info.cu -o bin/cuda_info
*/

#include <stdio.h>
#include <cuda.h>

/*
   GeForce GTX Titan @ 928 MHz
      SM: 14 * 192 sm/core = 2688 cores
      384-bit @ 3004 MHz = 288 GB/s

   GeForce GT 750M @ 925 MHz
      SM: 2 * 192 sm/core = 384 cores
      128-bit @ 2508 MHz = 80 GB/s

   GeForce GT 330M @ 1100 MHz
      SM: 6 * 8 sm/core = 48 cores
      128-bit @ 790 MHz = 25 GB/s
*/

int CudaGetCores( int major, int minor )
{
    int cores[] = {
          8,  8, 8, 8, 0,   0, // 1.0  1.1  1.2  1.3
         32, 48, 0, 0, 0,   0, // 2.0  2.1
        192,  0, 0, 0, 0, 192, // 3.0                 3.5
        256,  0, 0, 0, 0,   0
    };

    return cores[ 6*(major-1) + minor ];
}

// cudaDeviceProp
// Reference: http://developer.download.nvidia.com/compute/cuda/4_1/rel/toolkit/docs/online/group__CUDART__DEVICE_g5aa4f47938af8276f08074d09b7d520c.html
int main()
{
    int nDevices; 
    cudaError_t error = cudaGetDeviceCount( &nDevices );
    if( error != cudaSuccess )
        return printf( "ERROR: Couldn't find any CUDA devices!\n" );

    for( int iDevice = 0; iDevice < nDevices; iDevice++ )
    {
        cudaDeviceProp prop;

        cudaGetDeviceProperties( &prop, iDevice );

        printf( "\nGPU #%d: \'%s\' @ %d MHz\n", (iDevice+1), prop.name, (prop.clockRate/1000) );
        printf( "   Compute: %d.%d\n", prop.major, prop.minor );
        printf( "   Multi Processors: %d * %d Cores/SM = %d Cores\n"
           , prop.multiProcessorCount 
           , CudaGetCores( prop.major, prop.minor )
           , prop.multiProcessorCount * CudaGetCores( prop.major, prop.minor )
        );

        printf( "\n=== Memory ===\n" );
        printf( "   Total Memory : %lu MB (%lu bytes)\n", (prop.totalGlobalMem/1024)/1024, (size_t)prop.totalGlobalMem );
        printf( "   Bus Width    : %u-bit @ %d MHz ==> ", prop. memoryBusWidth, prop.memoryClockRate/1000 );
        printf( "   Max Bandwidth: %u GB/s\n"           , (prop.memoryClockRate/1000 * ((prop. memoryBusWidth/8)*2))/1000 ); // DDR2/3/4/5 = *2
        printf( "   Const memory : %lu (bytes)\n"       , prop.totalConstMem     );
        printf( "   Memory/Block : %lu\n"               , prop.sharedMemPerBlock );
        printf( "   Unified mem  : %d\n"                , prop.unifiedAddressing );

        printf( "\n=== Threads ===\n" );
        printf( "   Max Threads/SM : %d \n"       , prop.maxThreadsPerMultiProcessor );
        printf( "   Threads / Block: %d\n"        , prop.maxThreadsPerBlock );
        printf( "   Max Thread Size: %d, %d, %d\n", prop.maxThreadsDim[0], prop.maxThreadsDim[1], prop.maxThreadsDim[2] );
        printf( "   Max Grid size  : %u, %u, %u\n", prop.maxGridSize  [0], prop.maxGridSize  [1], prop.maxGridSize  [2] );
        printf( "   Registers/Block: %d\n"        , prop.regsPerBlock );

        printf( "\n=== Texture ===\n" );
        printf( "   Texture Size 1D: %d          \n", prop.maxTexture1D                                                );
        printf( "   Texture Size 2D: %d x %d     \n", prop.maxTexture2D[0], prop.maxTexture2D[1]                       );
        printf( "   Texture Size 3D: %d x %d x %d\n", prop.maxTexture3D[0], prop.maxTexture3D[1], prop.maxTexture3D[2] );

        printf( "\n" );
    }

    return 0;
}

