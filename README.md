# Buddhabrot

Single threaded (CPU), multi-threaded OpenMP (CPU), multi-threaded C++14 (CPU), multi-core using CUDA (GPU) {forthcoming}, and multi-core using OpenCL {forthcoming}, versions of [Buddhabrot](http://en.wikipedia.org/wiki/Buddhabrot)

This 2,160x2,880 at 32,768 depth [image](https://github.com/Michaelangel007/buddhabrot/blob/master/data/buddhabrot_2160x2880_32768.bmp) took 42 minutes on an i7 utilizing all 8 cores.

Here is a "thumbnail" at 1/2 size: 1080x1440

<img src="https://raw.githubusercontent.com/Michaelangel007/buddhabrot/master/data/buddhabrot_1080x1440_32768.png">

# Status

* Single threaded (CPU) - done
* Multi-threaded OpenMP - done
* Command line switch `-j#` to use a maximum number of threads - done!

= TODO =

* MSVC solution and project
* Multi-threaded C++14 -- not yet started
* Multi-core CUDA -- not yet started
* Multi-core OpenCL -- not yet started

                         +-------+------+---------+----------------+
                         | Linux | OSX  | Windows | Filename       |
        +----------------+-------+------+---------+----------------+
        | Single core    | Done  | Done | TODO    | bin/buddhabrot |
        | OpenMP         | Done  | Done | TODO    | bin/omp2       |
        | CUDA           | TODO  | TODO | TODO    | n/a            |
        | OpenCL         | TODO  | TODO | TODO    | n/a            |
        +----------------+-------+------+---------+----------------+

# Compiling

On Linux or OSX type: `make` The makefile will detect the OS and use the right flags.  Note that the CUDA versions are NOT built by default, you must use `make cuda`

For Windows, a MSVC (Microsoft Visual C) solution and project files will be provided soon.


# Running

To run the single threaded version `bin/buddhabrot`

To run the multi-threaded version `bin/omp3`


# Benchmarks

Using the default 1,024 x 768 at 1,000 depth we can see how much faster parallelizing the code can be:

        +--------------------+------+------+------+------+------+------+-----+
        | Hardware           | org. | cpu1 | omp1 | omp2 | omp3 | cuda | ocl |
        +--------------------+------+------+------+------+------+------+-----+
        | Intel i7 @ 2.6 GHz | 0:57 | 0:55 | 0:22 | 0:17 | 0:10 | n/a  | n/a |
        | AMD 955BE@ 3.5 GHz | 1:37 | 1:29 | 1:00 | 0:42 | 0:30 | n/a  | n/a |
        +--------------------+------+------+------+------+------+------+-----+
                                    (min:sec) (Lower is better)

        Legend:

        org.   Original Evercat version
        cpu1   Single core master reference
        omp1   Multi core (OpenMP) initial version 1
        omp2   Multi core (OpenMP) faster  version 2
        omp3   Multi core (OpenMP) fastest version 3
        omp4   Multi core (OpenMP) version 3 with float32 instead of float64 for comparison
        cuda   Multi core (CUDA  )
        ocl    Multi core (OpenCL)
        c14    Multi core (C++14)

## = Hardware =

Details on hardware used for building and testing:

        +--------------------+----------+-------------------------+---------------------+-------------------+
        | CPU                | Clock    | OS                      | RAM                 | GPU         Cores |
        +--------------------+----------+-------------------------+---------------------+-------------------+
        | Intel i7-4770K     @ 4040 MHz | Windows 7 Pro           | 32 GB DDR3 1600 Mhz | GTX 980Ti    2880 |
        | Intel i7           @ 2600 MHz | OSX 10.9                | 16 GB DDR3 1600 MHz | GT 750M       384 |
        | AMD PhenomII 955BE @ 3500 MHz | Linux, Ubuntu 12.04 LTS | 16 GB DDR3 1333 MHz | GTX Titan    2688 |
        +--------------------+----------+-------------------------+---------------------+-------------------+

## = Threads =

Using the shell script `jobs.sh` we can see how the numbers of threads effects time for `bin/omp3` on the AMD system:


                       +---+------+
                       | # | Time |
        +--------------+---+------+
        | bin/omp3 -j1 | 1 | 1:31 |
        | bin/omp3 -j2 | 2 | 0:47 |
        | bin/omp3 -j3 | 3 | 0:33 |
        | bin/omp3 -j4 | 4 | 0:30 |
        +--------------+---+------+

## = Depth =

Using the shell script `depth.sh` we can see how depth effects time on the AMD box:

                                     +-------+
                                     | Time  |
        +----------------------------+-------+
        | bin/omp2    4000 3000 4000 | ??:?? |
        | bin/omp2 -v 4000 3000 4000 | ??:?? |
        | bin/omp2    4000 3000 3000 | ??:?? |
        | bin/omp2 -v 4000 3000 3000 | ??:?? |
        | bin/omp2    4000 3000 2000 | ??:?? |
        | bin/omp2 -v 4000 3000 2000 | ??:?? |
        | bin/omp2    4000 3000 1000 | ??:?? |
        | bin/omp2 -v 4000 3000 1000 | ??:?? |
        +----------------------------+-------+

       /bin/omp2    4000 3000 2000   # 637 seconds, 10:37

# Tutorial: HOWTO Write a Multi-Core program

Let's convert a single core program, Buddhabrot, to a multi-core version.

Why Buddhabrot and not the more conventional Mandelbrot?

Two reasons: 

* Mandlebrot is trivial to parallelize while Buddhabrot is not, as a result Buddhabrot is less popular,
* Buddhabrot looks cooler then the over-used Mandelbrot fractal.

== Introduction to Mandelbrot ==

TL:DR; For those not familiar with Mandelbrot here is a quick refresher: Given a complex point we iterate "max depth" times and check to see if that point "escapes" -- that is stopping if its length is > 2.0.  If the current depth < max iterations then we plot it using a false color based on [0..depth].

In contradistinction the key difference between Mandelbort and Buddhabrot is that when a point "escapes" we plot a historgram of ALL the pixels of the "path" it took.   Buddhabrot touching many pixels makes this a slightly more complicated problem to parallelize.

Here are some more details in how Mandelbrot works:

We start with a 2D region. We use complex numbers to define the dimensions of this region.  The gist is that for each complex point in this region we do some processing to generate an image.

Given a complex point, P, we iterate the equation `Zn+1 = Zn^2 + P` a certain amount of times. We call the maximum number of iterations we process this equation the "maximum depth." If at any time the Euclidean length of this new point, |Zn+1|, is > 2.0 then we will denote the original point as "escaping" -- we will also note the "current depth" when this happened.  _How_ we plot this point depends on if we are rendering the Mandelbrot or Buddhabrot set:

With Mandelbrot:

If the current depth == max depth, then the original point doesn't escape and we _don't_ plot it -- we simple leave the background "as is."  However, if the current depth < max depth then the point P does "escape" and we plot a pixel with some false color based on the current depth and move on to processing the next point. Each complex point updates one and only one pixel in the output bitmap.  This is trivial to parallelize and is nice and fast.  The only "hard" part is is determining a "nice" color ramp you pick for pixel = [ 0 .. depth-1 ] which is what determines all those cool fractals. :-) End of the Mandelbrot story unless you were a [FracInt]( http://www.fractint.org/) user back in [the day.](https://en.wikipedia.org/wiki/Fractint)


## == Shut up and show me the (Mandelbrot) code! ==


Here is a small program to demonstrate how simple Mandelbrot is:

File: [text_mandelbrot.cpp](https://github.com/Michaelangel007/buddhabrot/blob/master/text_mandelbrot.cpp)

        #include <complex>
        #include <stdio.h>  // printf
        #include <string.h> // memset 

        typedef std::complex<double> complex;

        int MandelbrotCalculate( complex c, int max_depth )
        {
            // iterates z = z + c until |z| >= 2 or max_depth is reached,
            // returns the number of iterations.
            complex z = c;
            int depth = 0;

            for( depth = 0; depth < max_depth; ++depth )
            {
                // Optimization: Remove unnecessary square root by squaring both sides
                //if( std::abs(z) >= 2.0) break;
                if( (z.real()*z.real() + z.imag()*z.imag()) > 4.0) break;

                z = z*z + c;
            }

            return depth;
        }

        void MandelbrotPlot( int depth, int x, int y, char *output, int width )
        {
            static const char charset[] = ".,c8M@jawrpogOQEPGJ";
            char c = charset[ depth % (sizeof(charset)-1)];

            output[ (y*width) + x ] = c;
        }

        int main()
        {
            const int width = 78, height = 44, num_pixels = width*height;

            const complex center(-.7, 0),
                          span(2.7, -(4/3.0)*2.7*height/width), // 4/3 is aspect ratio
                          TopLeft = center - span/2.0;
            const int max_depth = 100000;

            char output[ height ][ width ];

            memset( output, (int)' ', sizeof( output ) );

            for(int pix=0; pix<num_pixels; ++pix)
            {
                const int x = pix%width, y = pix/width;

                complex p = TopLeft + complex(
                    x * span.real() / (width +1.0),
                    y * span.imag() / (height+1.0));

                int cur_depth = MandelbrotCalculate( p, max_depth );

                if (cur_depth != max_depth)
                {
                    MandelbrotPlot( cur_depth, x, y, (char*)output, width ); // output[y][x] = c
                }
            }

            // We could have a 1 character skirt on the right edge and bottom
            // but reserve one column and row for outputing.
            for( int y = 0; y < height-1; y++ )
                output[ y ][ width-1 ] = '\n';

            output[ height-1 ][ width-1 ] = 0;
            printf( "%s\n", &output[0][0] );

            return 0;
        }

Note: The original code used a costly square root with `std::abs(z) > 2` in MandelbrotCalculate().  The very first things we can do is optimize that out.

            for( depth = 0; depth < max_depth; ++depth )
            {
                // Optimization: Remove unnecessary square root by squaring both sides
                //if( std::abs(z) >= 2.0) break;
                if( (z.real()*z.real() + z.imag()*z.imag()) > 4.0) break;

                z = z*z + c;
            }

Moving on, let's take a quick look at an parallelized OpenMP version:

* http://bisqwit.iki.fi/story/howto/openmp/

It isn't quite as fast as it could be since it serializes the output, but it shoulds the mininmal number of changes we need to convert this from singel core to multi core.

File: mandelbrot.cpp

        #include <complex>
        #include <cstdio>
        #include <omp.h>

        typedef std::complex<double> complex;

        int MandelbrotCalculate(complex c, int maxiter)
        {
            // iterates z = z + c until |z| >= 2 or maxiter is reached,
            // returns the number of iterations.
            complex z = c;
            int n=0;

            for(; n<maxiter; ++n)
            {
                if( std::abs(z) >= 2.0) break;
                z = z*z + c;
            }
            return n;
        }

        int main()
        {
            const int width = 78, height = 44, num_pixels = width*height;

            const complex center(-.7, 0), span(2.7, -(4/3.0)*2.7*height/width);
            const complex begin = center-span/2.0, end = center+span/2.0;
            const int maxiter = 100000;

        #pragma omp parallel for ordered schedule(dynamic)
            for(int pix=0; pix<num_pixels; ++pix)
            {
                const int x = pix%width, y = pix/width;

                complex c = begin + complex(
                    x * span.real() / (width +1.0),
                    y * span.imag() / (height+1.0));

                int n = MandelbrotCalculate(c, maxiter);
                if(n == maxiter) n = 0;

            #pragma omp ordered
                {
                    char c = ' ';
                    if(n > 0)
                    {
                        static const char charset[] = ".,c8M@jawrpogOQEPGJ";
                        c = charset[n % (sizeof(charset)-1)];
                    }
                    std::putchar(c);
                    if(x+1 == width) std::puts("|");
                }
            }
        }



The Buddhabrot is mostly the same as Mandelbrot; the differences is how it is rendered:
 
                const int       iCol = iCel % nCol;
                const int       iRow = iCel / nCol;

                const double    x = gnWorldMinX + (iCol * dx);
                const double    y = gnWorldMinY + (iRow * dy);

                /* */ double    r = 0., i = 0., s, j;

                    for (int depth = 0; depth < gnMaxDepth; depth++)
                    {
                        s = (r*r - i*i) + x; // Zn+1 = Zn^2 + C<x,y>
                        j = (2.0*r*i)   + y;

                        r = s;
                        i = j;

                        if ((r*r + i*i) > 4.0) // escapes to infinity so trace path
                        {
                            plot( x, y, nWorld2ImageX, nWorld2ImageY, pTex, gnWidth, gnHeight, gnMaxDepth );
                            break;
                        }
                    }

                VERBOSE
                if (iTid == 0)
                {
                        const double percent = (100.0  * iCel) / nCel;

                        printf( "%6.2f%% = %d / %d%s", percent, iCel, nCel, gaBackspace );
                        fflush( stdout );
                }
            }


## = Single Core =

With Buddhabrot:

Things are slightly more complicated as we iterate the depth (potentially) twice for each point.

Let's say we have two points A and B in this region.

We iterate the complex Zn+1 = Zn^2 + A equation for A. Say after depth iterations we find it does NOT escape.  We don't plot anything for this point. We're done with A and move on.

We now process B and say we find after depth iterations it DOES escapes. Now this is where things become interesting. We _restart_ the equation again but this time for _each_ Zn+1 we plot it's current location. By plot we mean update a 2D histogram bitmap. This has has the effect of leaving a "ghost trail."

Now here's the multi-core problem: You give each point P its own thread.  Each point P is potentially updating hundreds of pixels.  Since it is a shared read-update-write resource we MUST synchronize access to the shared histogram bitmap, say using a critical section; if we don't then we'll get "dropped" pixels. On the other hand if we synchronize access we _completely_ kill performance!  Whoops!  Damn'd if you do, damn'd if you don't.


One minor optimization we can do with the Mandlebrot / Buddhabrot test "if escapes" is to remove that costly square root.


        if( sqrtf( r*r + i*i ) > 2.0 )

With the equivalent:

        if ((r*r + i*i) > 4.0)

This is what we have so far:

        int iCol = 0;
        for( double x = gnWorldMinX; x < gnWorldMaxX; x += dx )
        {
            iCol++;

            for( double y = gnWorldMinY; y < gnWorldMaxY; y += dy )
            {
                double r = 0., i = 0., s, j;
                for (int depth = 0; depth < gnMaxDepth; depth++)
                {
                    s = (r*r - i*i)) + x; // Zn+1 = Zn^2 + C<x,y>
                    j = (2*r*i)      + y;

                    r = s;
                    i = j;

                    if ((r*r + i*i) > 4.0) // escapes to infinity so trace path
                    {
                        plot( x, y, nWorld2ImageX, nWorld2ImageY, gpGreyscaleTexels, gnWidth, gnHeight, gnMaxDepth );
                        break;
                    }
                }
            }

Let's also display a percentage based on the columns we've completed.

            if( bDisplayProgress ) // Update % complete for each column
            {
                const double percent = (100.0  * iCol) / nCol;
                for( int i = 0; i < 40; i++ )
                    printf( "%c", 8 ); // ASCII backspace

                printf( "%6.2f%% = %d / %d", percent, iCel, nCel ); // iCol, nCol );
                fflush( stdout );
            }
        }

AMD: 1:29 (89 seconds)

While not impressive, we've already wittled off 10 seconds just by a little cleanup.


## = Multi Core (OpenMP) =

## == Pre-OpenMP cleanup ==

To properly convert from single-core to multi-core we first need to do a few things:

* When using OpenMP it can run a for() loop in parallel but ONLY if the loop index is an integer, not a floating-point value.  Hmm.

We convert from steping by dx and dy (floating-point) to stepping along the scaled width and scaled height (integers.)  This means we'll recompute the world space coordintes.

See, File: buddhabrot.cpp, Func: Buddhabrot()

        for( int iCol = 0; iCol < nCol; iCol++ )
        {
            const double x = gnWorldMinX + (dx*iCol);

            for( int iRow = 0; iRow < nRow; iRow++ )
            {
                const double y = gnWorldMinY + (dy*iRow);

                iCel++;

                double r = 0., i = 0., s, j;
                for (int depth = 0; depth < gnMaxDepth; depth++)
                {
                    s = (r*r - i*i) + x; // Zn+1 = Zn^2 + C<x,y>
                    j = (2.0*r*i)   + y;

                    r = s;
                    i = j;

                    if ((r*r) + (i*i) > 4.0) // escapes to infinity so trace path
                    {
                        plot( x, y, nWorld2ImageX, nWorld2ImageY, gpGreyscaleTexels, gnWidth, gnHeight, gnMaxDepth );
                        break;
                    }
                }
            }

            if( bDisplayProgress )
            {
                const double percent = (100.0  * iCel) / nCel;
                for( int i = 0; i < 40; i++ )
                    printf( "%c", 8 ); // ASCII backspace

                printf( "%6.2f%% = %d / %d", percent, iCel, nCel );
                fflush( stdout );
            }
        }


* Since we have N threads all fighting for contention over a single resource (greyscale output bitmap) we instead allocate N greyscale bitmaps so each thread has its own indepent local copy -- this will prevent costly `synchronization` fences / barries / waiting.

First, we'll need to extend our initial image allocation to allocate N images -- one for each thread.

Func: AllocImageMemory()

        // BEGIN OMP
            gnThreads = omp_get_num_procs();
            printf( "Detected: %d cores\n", gnThreads );

            for( int iThread = 0; iThread < gnThreads; iThread++ )
            {
                        gaThreadsTexels[ iThread ] = (uint16_t*) malloc( nGreyscaleBytes );
                memset( gaThreadsTexels[ iThread ], 0,                   nGreyscaleBytes );
            }
        // END OMP

Next, once we all done then we will then merge (add) all the thread's bitmap back into a single master image.

        // BEGIN OMP
            // 2. Gather
            const int nPix = gnWidth  * gnHeight; // Normal area
            for( int iThread = 0; iThread < gnThreads; iThread++ )
            {
                const uint16_t *pSrc = gaThreadsTexels[ iThread ];
                /* */ uint16_t *pDst = gpGreyscaleTexels;

                for( int iPix = 0; iPix < nPix; iPix++ )
                    *pDst++ += *pSrc++;
            }
        // END OMP


Now whenever writing any multi-core program it is best to have a reference single core version to "fall back" onto.  This will allow us to do a `diff` on the output (raw.data or .bmp) to check for divergence.  Translation: Detect bugs in the multi-core versions.

On our AMD platform the original version runs in 1:37 (97 seconds) -- this is mostly due to three things:

* is single threaded,
* excessive printf() calls,
* for() loop with extra counters (floating point and integer)

Since the original version has numerous bugs, lacks features, and is hard-coded we'll clean it up so the code is clean, clear, compact, and concise.  Along the way we'll also add multi-core support and provide timings of various hardware so we can see just how much faster each version becomes.


AMD time 0:42

        int Buddhabrot()
        {
            if( gnScale < 0)
                gnScale = 1;

            const int nCol = gnWidth  * gnScale ; // scaled width
            const int nRow = gnHeight * gnScale ; // scaled height

            const int nCel = nCol     * nRow    ; // scaled width  * scaled height;

            const double nWorldW = gnWorldMaxX - gnWorldMinX;
            const double nWorldH = gnWorldMaxY - gnWorldMinY;

            // Map Source (world space) to Pixels (image space)
            const double nWorld2ImageX = (double)(gnWidth  - 1.) / nWorldW;
            const double nWorld2ImageY = (double)(gnHeight - 1.) / nWorldH;

            const double dx = nWorldW / (nCol - 1.0);
            const double dy = nWorldH / (nRow - 1.0);

            // 1. Scatter

            // Linearize to 1D
        // BEGIN OMP
        #pragma omp parallel for
        // END OMP
            for( int iCel = 0; iCel < nCel; iCel++ )
            {
        // BEGIN OMP
                const int       iTid = omp_get_thread_num(); // Get Thread Index: 0 .. nCores-1
                /* */ uint16_t* pTex = gaThreadsTexels[ iTid ];
        // END OMP

                const int       iCol = iCel % nCol;
                const int       iRow = iCel / nCol;

                const double    x = gnWorldMinX + (iCol * dx);
                const double    y = gnWorldMinY + (iRow * dy);

                /* */ double    r = 0., i = 0., s, j;

                    for (int depth = 0; depth < gnMaxDepth; depth++)
                    {
                        s = (r*r - i*i) + x; // Zn+1 = Zn^2 + C<x,y>
                        j = (2.0*r*i)   + y;

                        r = s;
                        i = j;

                        if ((r*r + i*i) > 4.0) // escapes to infinity so trace path
                        {
                            plot( x, y, nWorld2ImageX, nWorld2ImageY, pTex, gnWidth, gnHeight, gnMaxDepth );
                            break;
                        }
                    }

                VERBOSE
        // BEGIN OMP
                if( (iTid == 0) && (iCel % gnWidth == 0) )
        // END OMP
                {
                    // We no longer need a critical section
                    // since we only allow thread 0 to print
                    {
                        const double percent = (100.0  * iCel) / nCel;

                        for( int i = 0; i < 40; i++ )
                            printf( "%c", 8 ); // ASCII backspace

                        printf( "%6.2f%% = %d / %d", percent, iCel, nCel );
                        fflush( stdout );
                    }
                }
            }

        // BEGIN OMP
            // 2. Gather
            const int nPix = gnWidth  * gnHeight; // Normal area
            for( int iThread = 0; iThread < gnThreads; iThread++ )
            {
                const uint16_t *pSrc = gaThreadsTexels[ iThread ];
                /* */ uint16_t *pDst = gpGreyscaleTexels;

                for( int iPix = 0; iPix < nPix; iPix++ )
                    *pDst++ += *pSrc++;
            }
        // END OMP

            return nCel;
        }



## == Multi-Core version 1 ==

When using OpenMP it is easy to miss some of the syntax for the #pragma's.

i.e. When you parallelize a for loop() there are 2 subtle but different outcomes.

    omp parallel     -- spawn a group of threads, for() excuted for each thread!
    omp parallel for -- divie loop amongst all threads, each thread does partial work

Using `omp parallel` won't produce the desired outcome.


        #pragma omp parallel for
            for( int iCol = 0; iCol < nCol; iCol++ )
            {
                const double x = gnWorldMinX + (dx*iCol);

        // BEGIN OMP
                int       iTid = omp_get_thread_num(); // Get Thread Index: 0 .. nCores-1
                uint16_t* pTex = gaThreadsTexels[ iTid ];
        // END OMP

                for( int iRow = 0; iRow < nRow; iRow++ )
                {
                    double y = gnWorldMinY + (dy*iRow);

        // BEGIN OMP
        #pragma omp atomic 
                    iPix++;
        // END OMP

                    double r = 0., i = 0., s, j;
                    for (int depth = 0; depth < gnMaxDepth; depth++)
                    {
                        s = ((r*r) - (i*i)) + x; // Zn+1 = Zn^2 + C<x,y>
                        j = (2*r*i)         + y;

                        r = s;
                        i = j;

                        if( (r*r + i*i) > 4.0) // escapes to infinity so trace path
                        {
                            plot( x, y, nWorld2ImageX, nWorld2ImageY, pTex, gnWidth, gnHeight, gnMaxDepth );
                            break;
                        }
                    }
                }
        // BEGIN OMP
                if( iTid > -1 ) // All threads can print but need to guard with a critical section
        // END OMP
                if( bDisplayProgress )
                {
                    double percent = (100.0  * iPix) / nPix;
        #pragma omp critical
                    {
                        for( int i = 0; i < 32; i++ )
                            printf( "%c", 8 ); // ASCII backspace

                        printf( "%6.2f%% = %d / %d", percent, iPix, nPix ); // iCol, nCol );
                        fflush( stdout );
                    }
                }
            }

On our AMD box we've gone from our initial 1:37 to 1:00 which isn't bad. How much faster is this?  The formulate is:

        % faster = 100 * (1 - (new time / old time))

        = 100 * 1 - (59 / 96)
        = 38.5% faster

Can we do better?

## == Faster Multi-Core? ==

Inspecting the CPU load (Linux: System Monitor, OSX: Activity Monitor, Windows: Task Manager) we notice that only a few cores tend to be pegged. :-(

Why?

The key to good multi-core performance is load balancing. We have 2 loops but we're only effectively parallelizing the outer.  Ack!

If we "linearize" the 2D loop into a 1D loop we can achieve better CPU utilization; we are breaking large tasks down into smaller ones that can be scheduled across the cores.

            // 1. Scatter

            // Linearize to 1D
        #pragma omp parallel for
            for( int iCel = 0; iCel < nCel; iCel++ )
            {
        // BEGIN OMP
                const int       iTid = omp_get_thread_num(); // Get Thread Index: 0 .. nCores-1
                /* */ uint16_t* pTex = gaThreadsTexels[ iTid ];
        // END OMP

                const int       iCol = iCel % nCol;
                const int       iRow = iCel / nCol;

                const double    x = gnWorldMinX + (iCol * dx);
                const double    y = gnWorldMinY + (iRow * dy);

                /* */ double    r = 0., i = 0., s, j;

                    for (int depth = 0; depth < gnMaxDepth; depth++)
                    {
                        s = (r*r - i*i) + x; // Zn+1 = Zn^2 + C<x,y>
                        j = (2*r*i)     + y;

                        r = s;
                        i = j;

                        if  ((r*r + i*i) > 4.0) // escapes to infinity so trace path
                        {
                            plot( x, y, nWorld2ImageX, nWorld2ImageY, pTex, gnWidth, gnHeight, gnMaxDepth );
                            break;
                        }
                    }

                if( iTid == 0 )
                if( bDisplayProgress )
                {
                    {
                        const double percent = (100.0  * iCel) / nCel;
                        for( int i = 0; i < 40; i++ )
                            printf( "%c", 8 ); // ASCII backspace

                        printf( "%6.2f%% = %d / %d", percent, iCel, nCel );
                        fflush( stdout );
                    }
                }
            }

        // BEGIN OMP
            // 2. Gather
            const int nPix = gnWidth  * gnHeight; // Normal area
            for( int iThread = 0; iThread < gnThreads; iThread++ )
            {
                const uint16_t *pSrc = gaThreadsTexels[ iThread ];
                /* */ uint16_t *pDst = gpGreyscaleTexels;

                for( int iPix = 0; iPix < nPix; iPix++ )
                    *pDst++ += *pSrc++;
            }
        // END OMP

We try out the new code `bin/omp2` and see we have a time of 0:42 seconds which looks great!

        % faster = 100 * (1 - (new time / old time))

        = 100 * 1 - (47 / 96)
        = 51.04% faster

Except there is one minor performance penalty. What!?

Hint: It is with output.

Notice that if we are displaying the progress then for _every_ cell update we are calling printf() and fflush(). This means we are spending enormous amounts of time outputting the progress -- all this overheads adds up! Ideally we would check every N cells. Just how bad is this?

For example, on the i7

        bin/omp2     # 17 seconds, 0:17
        bin/omp2 -v  # 73 seconds, 1:13

Whoa!

Fortunately the fix is simple:

From:

        if( bDisplayProgress )
        if( iTid == 0 )

To:

        if( bDisplayProgress )
        if( (iTid == 0) && (iCel % gnWidth == 0) )

With output on the i7 we're back down to:

        bin/omp2 -v  # 22 seconds, 0:22

However we _still_ have a problem.  For some reason on OSX the progress will "stall" at 12.50%.

In order to have an accurate progress with multi-core we must use an atomic counter.

        #pragma omp atomic
            iPix++;

Fortunately atomics are relatively cheap. Here is the final version:

            // 1. Scatter

            // Linearize to 1D
        // BEGIN OMP
        #pragma omp parallel for
        // END OMP
            for( int iPix = 0; iPix < nCel; iPix++ )
            {
        // BEGIN OMP
        #pragma omp atomic
                iCel++;

                const int       iTid = omp_get_thread_num(); // Get Thread Index: 0 .. nCores-1
                /* */ uint16_t* pTex = gaThreadsTexels[ iTid ];
        // END OMP

                const int       iCol = iCel % nCol;
                const int       iRow = iCel / nCol;

                const double    x = gnWorldMinX + (iCol * dx);
                const double    y = gnWorldMinY + (iRow * dy);

                /* */ double    r = 0., i = 0., s, j;

                    for (int depth = 0; depth < gnMaxDepth; depth++)
                    {
                        s = (r*r - i*i) + x; // Zn+1 = Zn^2 + C<x,y>
                        j = (2.0*r*i)   + y;

                        r = s;
                        i = j;

                        if ((r*r + i*i) > 4.0) // escapes to infinity so trace path
                        {
                            plot( x, y, nWorld2ImageX, nWorld2ImageY, pTex, gnWidth, gnHeight, gnMaxDepth );
                            break;
                        }
                    }

                VERBOSE
        // BEGIN OMP
                if (iTid == 0)
        // END OMP
                {
                    // We no longer need a critical section
                    // since we only allow thread 0 to print
                    {
                        const double percent = (100.0  * iCel) / nCel;

                        printf( "%6.2f%% = %d / %d%s", percent, iCel, nCel, gaBackspace );
                        fflush( stdout );
                    }
                }
            }


AMD time 0:42

        int Buddhabrot()
        {
            if( gnScale < 0)
                gnScale = 1;

            const int nCol = gnWidth  * gnScale ; // scaled width
            const int nRow = gnHeight * gnScale ; // scaled height

            const int nCel = nCol     * nRow    ; // scaled width  * scaled height;

            const double nWorldW = gnWorldMaxX - gnWorldMinX;
            const double nWorldH = gnWorldMaxY - gnWorldMinY;

            // Map Source (world space) to Pixels (image space)
            const double nWorld2ImageX = (double)(gnWidth  - 1.) / nWorldW;
            const double nWorld2ImageY = (double)(gnHeight - 1.) / nWorldH;

            const double dx = nWorldW / (nCol - 1.0);
            const double dy = nWorldH / (nRow - 1.0);

            // 1. Scatter

            // Linearize to 1D
        // BEGIN OMP
        #pragma omp parallel for
        // END OMP
            for( int iCel = 0; iCel < nCel; iCel++ )
            {
        // BEGIN OMP
                const int       iTid = omp_get_thread_num(); // Get Thread Index: 0 .. nCores-1
                /* */ uint16_t* pTex = gaThreadsTexels[ iTid ];
        // END OMP

                const int       iCol = iCel % nCol;
                const int       iRow = iCel / nCol;

                const double    x = gnWorldMinX + (iCol * dx);
                const double    y = gnWorldMinY + (iRow * dy);

                /* */ double    r = 0., i = 0., s, j;

                    for (int depth = 0; depth < gnMaxDepth; depth++)
                    {
                        s = (r*r - i*i) + x; // Zn+1 = Zn^2 + C<x,y>
                        j = (2.0*r*i)   + y;

                        r = s;
                        i = j;

                        if ((r*r + i*i) > 4.0) // escapes to infinity so trace path
                        {
                            plot( x, y, nWorld2ImageX, nWorld2ImageY, pTex, gnWidth, gnHeight, gnMaxDepth );
                            break;
                        }
                    }

                VERBOSE
        // BEGIN OMP
                if( (iTid == 0) && (iCel % gnWidth == 0) )
        // END OMP
                {
                    // We no longer need a critical section
                    // since we only allow thread 0 to print
                    {
                        const double percent = (100.0  * iCel) / nCel;

                        for( int i = 0; i < 40; i++ )
                            printf( "%c", 8 ); // ASCII backspace

                        printf( "%6.2f%% = %d / %d", percent, iCel, nCel );
                        fflush( stdout );
                    }
                }
            }

        // BEGIN OMP
            // 2. Gather
            const int nPix = gnWidth  * gnHeight; // Normal area
            for( int iThread = 0; iThread < gnThreads; iThread++ )
            {
                const uint16_t *pSrc = gaThreadsTexels[ iThread ];
                /* */ uint16_t *pDst = gpGreyscaleTexels;

                for( int iPix = 0; iPix < nPix; iPix++ )
                    *pDst++ += *pSrc++;
            }
        // END OMP

            return nCel;
        }


## == Fastest Multi-Core? ==

Is this the fastest we can do? Believe it or not we can further tweak the inner loop:

        // @return Number of input scaled pixels (Not uber total of all pixels processed)
        // ========================================================================
        int Buddhabrot()
        {
            if( gnScale < 0)
                gnScale = 1;

            const int nCol = gnWidth  * gnScale ; // scaled width
            const int nRow = gnHeight * gnScale ; // scaled height

            /* */ int iCel = 0                  ; // Progress status for percent compelete
            const int nCel = nCol     * nRow    ; // scaled width  * scaled height;

            const double nWorldW = gnWorldMaxX - gnWorldMinX;
            const double nWorldH = gnWorldMaxY - gnWorldMinY;

            // Map Source (world space) to Pixels (image space)
            const double nWorld2ImageX = (double)(gnWidth  - 1.) / nWorldW;
            const double nWorld2ImageY = (double)(gnHeight - 1.) / nWorldH;

            const double dx = nWorldW / (nCol - 1.0);
            const double dy = nWorldH / (nRow - 1.0);

        // BEGIN OMP
            // 1. Scatter

            // Linearize to 1D
        #pragma omp parallel for
        // END OMP
            for( int iPix = 0; iPix < nCel; iPix++ )
            {
        // BEGIN OMP
        #pragma omp atomic
                iCel++;

                const int       iTid = omp_get_thread_num(); // Get Thread Index: 0 .. nCores-1
                /* */ uint16_t* pTex = gaThreadsTexels[ iTid ];
        // END OMP

                const int       iCol = iCel % nCol;
                const int       iRow = iCel / nCol;

                const double    x = gnWorldMinX + (iCol * dx);
                const double    y = gnWorldMinY + (iRow * dy);

                /* */ double    r = 0., i = 0., s, j;

                    for (int depth = 0; depth < gnMaxDepth; depth++)
                    {
                        s = (r*r - i*i) + x; // Zn+1 = Zn^2 + C<x,y>
                        j = (2.0*r*i)   + y;

                        r = s;
                        i = j;

                        if ((r*r + i*i) > 4.0) // escapes to infinity so trace path
                        {
                            plot( x, y, nWorld2ImageX, nWorld2ImageY, pTex, gnWidth, gnHeight, gnMaxDepth );
                            break;
                        }
                    }

                VERBOSE
        // BEGIN OMP
                if (iTid == 0)
        // END OMP
                {
                    // We no longer need a critical section
                    // since we only allow thread 0 to print
                    {
                        const double percent = (100.0  * iCel) / nCel;

                        printf( "%6.2f%% = %d / %d%s", percent, iCel, nCel, gaBackspace );
                        fflush( stdout );
                    }
                }
            }

        // BEGIN OMP
            // 2. Gather
            const int nPix = gnWidth  * gnHeight; // Normal area
            for( int iThread = 0; iThread < gnThreads; iThread++ )
            {
                const uint16_t *pSrc = gaThreadsTexels[ iThread ];
                /* */ uint16_t *pDst = gpGreyscaleTexels;

                for( int iPix = 0; iPix < nPix; iPix++ )
                    *pDst++ += *pSrc++;
            }
        // END OMP

            return nCel;
        }

Timings:

   i7 : 0:10 seconds
   AMD: 0:28 seconds

Our third tweak gets our best time on AMD down to:

        % faster = 100 * (1 - (new time / old time))

        = 100 * 1 - (28 / 96)
        = 70.83% faster


Thus our final time on the i7 is 0:10 !

If we turn on output, our time goes down to ...

Thus we see that there is always a trade-off between raw speed and displaying a progress.


## == A matter of precision? ==

Since we are using `doubles` what would happen if we used `floats`?

File: `buddhabrot_omp4.cpp`

Luckily Modern CPU's don't have a float64 penalty compared to float32.  When we run this on the GPU we'll revisit this difference. i.e. Modern GPU float:double performance can be as little as 1:3 in high end graphics cards such as the original nVidia GTX Titan, or as high as 1:24 in "gaming" graphics cards such as the nVidia GTX 980.


## = Verification =

Ther is a shell script `verify.sh` (Linux, OSX) that will very the raw output for the various versions.


## Conclusions

Parallelzing an algorithm is not always possible.  Sometimes we have to think "out of the box" to do so.  Hopefully this tutorial has inspired you to add multi-core 

# Utilities

* raw2bmp Convert a raw 16-bit .data to .bmp
* cuda-info Display # of cores on your nVidia GPU

# References

* PDF: [A "Hands-on" Introduction to OpenMP](http://openmp.org/mp-documents/omp-hands-on-SC08.pdf)
* PDF: [OpenMP by Example](http://people.math.umass.edu/~johnston/PHI_WG_2014/OpenMPSlides_tamu_sc.pdf)
* HTML: https://computing.llnl.gov/tutorials/openMP/
* PPT: http://www.cs.berkeley.edu/~mhoemmen/cs194/Lectures/openmp-tutorial.ppt
* https://software.intel.com/en-us/node/527527
* http://stackoverflow.com/questions/14016026/openmp-and-pragma-omp-atomic

# Misc. Links

* CUDA-Z http://cuda-z.sourceforge.net


