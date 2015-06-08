/*
Compile:
    g++ -O2 text_buddhabrot.cpp -o bin/text_buddhabrot

Heavily cleaned up version based on Mandelbrot at:

* http://bisqwit.iki.fi/story/howto/openmp/

*/

    #include <complex>
    #include <stdio.h>  // printf
    #include <string.h> // memset 

    typedef std::complex<double> complex;

    int BuddhabrotCalculate( complex c, int max_depth )
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

    void BuddhabrotPlot( complex p, int depth, int x, int y, char *output, int width, int height )
    {
        (void)height;

        static const char charset[] = ".,c8M@jawrpogOQEPGJ";
        char c = charset[ depth % (sizeof(charset)-1)];

        complex z = p;

        {
            int u = z.real();
            int v = z.imag();

            if( (u >= 0) && (v >= 0) && (u < width) && (v < height) )
            {
            }

            output[ (y*width) + x ] = c;
        }
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

            int cur_depth = BuddhabrotCalculate( p, max_depth );

            if (cur_depth != max_depth)
            {
                BuddhabrotPlot( p, cur_depth, x, y, (char*)output, width, height ); // output[ y ][ x ] = c;
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
