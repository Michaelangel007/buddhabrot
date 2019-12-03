/*  Buddhabrot
    https://github.com/Michaelangel007/buddhabrot
    http://en.wikipedia.org/wiki/User_talk:Michael.Pohoreski/Buddhabrot.cpp

    Optimized and cleaned up version by Michael Pohoreski
    Based on the original version by Evercat

        g++ -Wall -O2 buddhabrot.cpp -o buddhabrot
        buddhabrot 4000 3000 20000

   Released under the GNU Free Documentation License
   or the GNU Public License, whichever you prefer.
*/

#if _WIN32
    #define _CRT_SECURE_NO_WARNINGS 1
#endif

// Includes
    #include <stdio.h>
    #include <stdlib.h>
    #include <math.h>
    #include <stdint.h> // uint16_t uint32_t
    #include <string.h> // memset()
// BEGIN C++11
    #include <thread>
    #include "util_threads.h"
// END C++11

#ifdef _MSC_VER
    // stupid MS ignoring standards yet again
    // http://stackoverflow.com/questions/2915672/snprintf-and-visual-studio-2010
    #define snprintf _snprintf
#endif

// Macros
    #define VERBOSE if(gbVerbose)

// Globals

    // Input parameters
    double    gnWorldMinX        = -2.102613; // WorldW = MaxX-MinX = 3.303226
    double    gnWorldMaxX        =  1.200613;
    double    gnWorldMinY        = -1.237710; // WorldH = MaxY-MinY = 2.47742 
    double    gnWorldMaxY        =  1.239710;

    int       gnMaxDepth         = 1000; // max number of iterations == # of pixels to plot per complex number
    int       gnWidth            = 1024; // image width
    int       gnHeight           =  768; // image height
    int       gnScale            =   10; // not sub-sample, but sub-divide world width / (pixel width * scale)

    bool      gbAutoBrightness   = false;
    // Default MaxDepth = 1000 @ 1042x768 has a maximum greyscale intensity = 5010 -> 230/5010 = filter out bottom 4.590808% of image as black
    int       gnGreyscaleBias    = -230; // color pixel = (greyscale pixel + bias) * scale = 5010 - 230 = 4780

    float     gnScaleR           = 0.09f; // Default: (5010 - 230) * 0.09 = 430.2
    float     gnScaleG           = 0.11f; // Default: (5010 - 230) * 0.11 = 525.8
    float     gnScaleB           = 0.18f; // Default: (5010 - 230) * 0.18 = 860.4

    bool      gbVerbose          = false;
    bool      gbSaveRawGreyscale = true ;
    bool      gbRotateOutput     = true ;
    bool      gbSaveBMP          = true ;

    // Calculated/Cached
    uint32_t  gnImageArea        =    0; // image width * image height

    // Output
    uint16_t *gpGreyscaleTexels  = NULL; // [ height ][ width ] 16-bit greyscale
    uint8_t  *gpChromaticTexels  = NULL; // [ height ][ width ] 24-bit RGB

    const int BUFFER_BACKSPACE   = 64;
    char      gaBackspace[ BUFFER_BACKSPACE ];

    char     *gpFileNameBMP      = 0; // user over-ride default?
    char     *gpFileNameRAW      = 0; // user over-ride default?


// Timer___________________________________________________________________________ 

// Implementation _________________________________________________________________ 

// ========================================================================
void AllocImageMemory( const int width, const int height )
{
    gnImageArea = width * height;

    const size_t nGreyscaleBytes = gnImageArea  * sizeof( uint16_t );
    gpGreyscaleTexels = (uint16_t*) malloc( nGreyscaleBytes );          // 1x 16-bit channel: W
    memset( gpGreyscaleTexels, 0, nGreyscaleBytes );

    const size_t chromaticBytes  = gnImageArea * 3 * sizeof( uint8_t ); // 3x 8-bit channels: R,G,B
    gpChromaticTexels = (uint8_t*) malloc( chromaticBytes );
    memset( gpChromaticTexels, 0, chromaticBytes );

    for( int i = 0; i < (BUFFER_BACKSPACE-1); i++ )
        gaBackspace[ i ] = 8; // ASCII backspace

    gaBackspace[ BUFFER_BACKSPACE-1 ] = 0;

// BEGIN C++11
    if(!gnThreadsActive) // user didn't specify how many threads to use, default to all of them
        gnThreadsActive = gnThreadsMaximum;
    else
        ; //  only use 'gnThreadsActive' threads

    for( int iThread = 0; iThread < gnThreadsActive; iThread++ )
    {
                gaThreadsTexels[ iThread ] = (uint16_t*) malloc( nGreyscaleBytes );
        memset( gaThreadsTexels[ iThread ], 0,                   nGreyscaleBytes );
    }
// END C++11
}


// ========================================================================
int main( int nArg, char * aArg[] )
{
// BEGIN C++11
    gnThreadsMaximum = std::thread::hardware_concurrency();
    if( gnThreadsMaximum > MAX_THREADS )
        gnThreadsMaximum = MAX_THREADS;
// END C++11


printf( "Auto-detect Threads: %u\n", gnThreadsMaximum );
    if ((gnThreadsMaximum <    1)
    ||  (gnThreadsMaximum > 1024))
    {
        printf( "ERROR: Unable to get maximum number of threads!\n" );
        return -1;
    }

// :

    printf( "Width: %d  Height: %d  Depth: %d  Scale: %d  RotateBMP: %d  SaveRaw: %d\n", gnWidth, gnHeight, gnMaxDepth, gnScale, gbRotateOutput, gbSaveRawGreyscale );

    AllocImageMemory( gnWidth, gnHeight );

// BEGIN C++11
    printf( "Using: %u / %u cores\n", gnThreadsActive, gnThreadsMaximum );
// END C++11

    return 0;
}


