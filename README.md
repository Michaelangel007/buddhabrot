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


# Benchmarks (min:sec)

Using the default 1024x768 with 1,000 depth we can see how much faster parallelizing the code can be:

        +--------------+------+------+------+------+------+-----+
        | Hardware     | org  | cpu1 | omp1 | omp2 | cuda | ocl |
        +--------------+------+------+------+------+------+-----+
        | i7 @ 2.6 GHz | 0:57 | 0:55 | 0:21 | 0:17 | n/a  | n/a |
        | 955@ 3.5 GHz | 1:37 | 1:29 | 1:00 | 0:42 | n/a  | n/a |
        +--------------+------+------+------+------+------+-----+

= Hardware =

* Intel i7 @ 2.6 GHz, OSX 10.9, 16 GB DDR3 1600 MHz, GT 750M (Mobile) (Cores: 384)
* AMD Phenom II 955BE @ 3.5 GHz, 16 GB DDR3 PC3 12800, Linux, Ubuntu 12.04 LTS, GTX Titan (Discrete) (Cores: 2688)

= Legend: =

    cpu1   Single core master reference
    omp1   Multi core (OpenMP) initial version 1
    omp2   Multi core (OpenMP) faster  version 2
    cuda   Multi core (CUDA  )
    ocl    Multi core (OpenCL)

# Tutorial

Let's convert a single core program, buddhabrot, to a multi-core version.

Along the way we'll provide timings of various hardware so we can see just how much faster each version becomes.

On our AMD platform the original version runs in 1:37 -- this is mostly due to two things:

a) excessive printf
b) for() loop with extra counters (floating point and integer)

Since the original version has numerous bugs, we'll clean it up.

# = Single Core =

Whenever writing any multi-core programs it is best to have a reference single core version.

This will allow us to do a `diff` on the output (raw.data or .bmp) to check for divergence (i.e. bugs in the multi-core versions.)

We display a percentage based on the columns we've completed.

        int iCol = 0;
        for( double x = gnWorldMinX; x < gnWorldMaxX; x += dx )
        {
            iCol++;

            for( double y = gnWorldMinY; y < gnWorldMaxY; y += dy )
            {

                double r = 0., i = 0., s, j;
                for (int depth = 0; depth < gnMaxDepth; depth++)
                {
                    s = ((r*r) - (i*i)) + x; // Zn+1 = Zn^2 + C<x,y>
                    j = (2*r*i)         + y;

                    r = s;
                    i = j;

                    if( (r*r + i*i) > 4.0) // escapes to infinity so trace path
                    {
                        plot( x, y, nWorld2ImageX, nWorld2ImageY, gpGreyscaleTexels, gnWidth, gnHeight, gnMaxDepth );
                        break;
                    }
                }
            }

            VERBOSE
            {
                const double percent = (100.0  * iCol) / nCol;
                for( int i = 0; i < 32; i++ )
                    printf( "%c", 8 ); // ASCII backspace

                printf( "%6.2f%% = %d / %d", percent, iCel, nCel ); // iCol, nCol );
                fflush( stdout );
            }
        }

AMD: 89 seconds 

== Pre-OpenMP cleanup ==

To convert from single-core to multi-core we first need to do a few things:

a) When using OpenMP it can run a foor() loop in paralle
but ONLY if the loop index is an integer, not a floating-point.

We convert steping by dx to stepping along the scaled width and scaled height.

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
                    s = ((r*r) - (i*i)) + x; // Zn+1 = Zn^2 + C<x,y>
                    j = (2*r*i)         + y;

                    r = s;
                    i = j;

                    if ((r*r) + (i*i) > 4.0) // escapes to infinity so trace path
                    {
                        plot( x, y, nWorld2ImageX, nWorld2ImageY, gpGreyscaleTexels, gnWidth, gnHeight, gnMaxDepth );
                        break;
                    }
                }
            }

            VERBOSE
            {
                const double percent = (100.0  * iCel) / nCel;
                for( int i = 0; i < 40; i++ )
                    printf( "%c", 8 ); // ASCII backspace

                printf( "%6.2f%% = %d / %d", percent, iCel, nCel );
                fflush( stdout );
            }
        }


b)  Since we have N threads all fighting for contention over a single greyscale output bitmap
We allocate N greyscale bitmaps so each thread has its own indepent local copy -- this will prevent costly `synchronization` fences / barries / waiting.
We will then merge (add) all the thread's bitmap back into a single master copy.


== Multi-Core ==

You need to be careful to specify 'for' in the #pragma

Note:
    omp parallel     -- spawn a group of threads, for() excuted for each thread!
    omp parallel for -- divie loop amongst all threads, each thread does partial work


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
                VERBOSE
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

AMD: 1:00 (60 seconds) 

= Faster Multi-Core? =

Inspecting the CPU load (Linux: System Monitor, OSX: Activity Monitor, Windows: Task Manager) we notice that only a few cores tend to be pegged.

Why?

The key to good multi-core performance is load balancing. We have 2 loops but we're only effectively parallelizing the outer.

If we linearize the 2D loop into a 1D loop we can achieve better CPU utilization; we are breaking the large tasks down into smaller ones.

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
        #pragma omp critical
                            plot( x, y, nWorld2ImageX, nWorld2ImageY, pTex, gnWidth, gnHeight, gnMaxDepth );
                            break;
                        }
                    }

                if( iTid == 0 )
                VERBOSE
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


# Verification

Ther is a shell script `verify.sh` (Linux, OSX) that will very the raw output for the various versions.


# References

* PDF: [A "Hands-on" Introduction to OpenMP](http://openmp.org/mp-documents/omp-hands-on-SC08.pdf)
* PDF: [OpenMP by Example](http://people.math.umass.edu/~johnston/PHI_WG_2014/OpenMPSlides_tamu_sc.pdf)
* HTML: https://computing.llnl.gov/tutorials/openMP/
* PPT: http://www.cs.berkeley.edu/~mhoemmen/cs194/Lectures/openmp-tutorial.ppt
