# Buddhabrot

Single core, multi core, CUDA, and OpenCL versions of [Buddhabrot](en.wikipedia.org/wiki/Buddhabrot)

# Status

* Single core - Done!
* Multi core (OpenMP) - Done!
* Multi core (CUDA) - not yet started
* Multi core (OpenCL) - not yet started

                         +-------+------+---------+----------------+
                         | Linux | OSX  | Windows | Filename       |
        +----------------+-------+------+---------+----------------+
        | Single core    | Done  | Done | TODO    | bin/buddhabrot |
        | OpenMP         | Done  | Done | TODO    | bin/omp2       |
        | CUDA           | TODO  | TODO | TODO    | n/a            |
        | OpenCL         | TODO  | TODO | TODO    | n/a            |
        +----------------+-------+------+---------+----------------+

# Compiling

   On Linux or OSX type: `make`

   The makefile will detect the OS and use the right flags.

# Benchmarks (min:sec) (Lower is Better)

Using the default 1024x768 at 1,000 depth we can see how much faster parallelizing the code can be:

        +--------------+------+------+------+------+------+------+-----+
        | Hardware     | org. | cpu1 | omp1 | omp2 | omp3 | cuda | ocl |
        +--------------+------+------+------+------+------+------+-----+
        | i7 @ 2.6 GHz | 0:57 | 0:55 | 0:21 | 0:17 | 0:10 | n/a  | n/a |
        | 955@ 3.5 GHz | 1:37 | 1:29 | 1:00 | 0:42 | 0:28 | n/a  | n/a |
        +--------------+------+------+------+------+------+------+-----+

= Legend: =

    org.   Original Evercat version
    cpu1   Single core master reference
    omp1   Multi core (OpenMP) initial version 1
    omp2   Multi core (OpenMP) faster  version 2
    omp3   Multi core (OpenMP) fastest version 3
    cuda   Multi core (CUDA  )
    ocl    Multi core (OpenCL)

How depth effects time:

       /bin/omp2    4000 3000 4000
       /bin/omp2 -v 4000 3000 4000

       /bin/omp2    4000 3000 2000   # 637 seconds, 10:37
       /bin/omp2 -v 4000 3000 2000

       /bin/omp2    4000 3000 1000
       /bin/omp2 -v 4000 3000 1000

= Hardware =

* Intel i7 @ 2.6 GHz, OSX 10.9, 16 GB DDR3 1600 MHz, GT 750M (Mobile) (Cores: 384)
* AMD Phenom II 955BE @ 3.5 GHz, 16 GB DDR3 1333 MHz (PC3 12800), Linux, Ubuntu 12.04 LTS, GTX Titan (Discrete) (Cores: 2688)

# Tutorial: HOWTO Write a Multi-Core program

Let's convert a single core program, Buddhabrot, to a multi-core version.

Why Buddhabrot and not the more conventional Mandelbrot?

Two reasons: 

* Mandlebrot is trivial to parallelize while Buddhabrot is not, and
* as a result Buddhabrot is less popular; this makes it more interesting.

Here is a small program to demonstrate how simple Mandelbrot is:

Note: It uses a costly square root with std::abs(z), we'll optimize that out in the Buddhabrot code.

* http://bisqwit.iki.fi/story/howto/openmp/

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

Along the way we'll provide timings of various hardware so we can see just how much faster each version becomes.

On our AMD platform the original version runs in 1:37 (97 seconds) -- this is mostly due to three things:

* is single threaded,
* excessive printf() calls,
* for() loop with extra counters (floating point and integer)

Since the original version has numerous bugs, lacks features, and is hard-coded we'll clean it up so the code is clean, clear, compact, and concise.  Along the way we'll also add multi-core support.

# = Single Core =

Now whenever writing any multi-core programs it is best to have a reference single core version.

This will allow us to do a `diff` on the output (raw.data or .bmp) to check for divergence (i.e. bugs in the multi-core versions.)

The Buddhabrot generation is basically a modified Mandelbrot. For those not familiar with Mandelbrot here is a quick refresher: Given a complex point we iterate "depth" times and check to see if that point "escapes" -- that is, its length is > 2.0.  If it does escape then we plot a black pixel else we note the depth [0..depth] and false color it based on the depth.

In contradistinction the key difference between Mandelbort and Buddhabrot is that when a point "escapes" we plot ALL the pixels of the "path" it took.   Buddhabrot touching many pixels makes this a slightly more complicated problem to parallelize.

One minor optimization we can do with the Mandlebrot / Buddhabrot test "if esscapes" is to remove that costly square root.


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


# = Multi Core (OpenMP) =

# == Pre-OpenMP cleanup ==

To properly convert from single-core to multi-core we first need to do a few things:

* When using OpenMP it can run a foor() loop in parallel but ONLY if the loop index is an integer, not a floating-point value.  Hmm.

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



# == Multi-Core version 1 ==

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

        = 100 * 1 - (60 / 96)
        = 37.5% faster

Can we do better?

# == Faster Multi-Core? ==

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


Our best time on the AMD is:

        % faster = 100 * (1 - (new time / old time))

        = 100 * 1 - (28 / 96)
        = 70.83% faster


Thus our final time on the i7 is 0:10 !


Thus we see that there is always a trade-off between raw speed and displaying a progress.


# = Verification =

Ther is a shell script `verify.sh` (Linux, OSX) that will very the raw output for the various versions.


# References

* PDF: [A "Hands-on" Introduction to OpenMP](http://openmp.org/mp-documents/omp-hands-on-SC08.pdf)
* PDF: [OpenMP by Example](http://people.math.umass.edu/~johnston/PHI_WG_2014/OpenMPSlides_tamu_sc.pdf)
* HTML: https://computing.llnl.gov/tutorials/openMP/
* PPT: http://www.cs.berkeley.edu/~mhoemmen/cs194/Lectures/openmp-tutorial.ppt
* https://software.intel.com/en-us/node/527527
* http://stackoverflow.com/questions/14016026/openmp-and-pragma-omp-atomic
