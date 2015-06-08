/*

Compile:
    g++ -O2 mandelbrot.cpp -o bin/mandelbrot

    g++ -Wall -Wextra -O2 -DOMP mandelbrot.cpp -o bin/mandelbrot_omp -fopenmp

Reference:
* http://bisqwit.iki.fi/story/howto/openmp/

Output:
          ,,,,,,,cccccccccccccccccccccccc888888888MMM@jprajwaM888888cccccc,,,,|
         ,,,,,,,ccccccccccccccccccccccc8888888888MMM@@jwoOQrj@M8888888cccccc,,|
         ,,,,,cccccccccccccccccccccccc8888888888MMMM@jaroEpaj@MM8888888cccccc,|
        ,,,,,ccccccccccccccccccccccc8888888888MMMM@@aOrOJ .raj@MMM888888cccccc|
       ,,,,,cccccccccccccccccccccc88888888888MMM@@@wQQGa  MO8w@MMMMM88888ccccc|
       ,,,ccccccccccccccccccccccc8888888888MMM@@@jjarP      @wj@@MMMMM8888cccc|
      ,,,cccccccccccccccccccccc8888888888MM@@@@@jjawo@      Qwaj@@@@MMMM88cccc|
      ,,cccccccccccccccccccccc88888888MMM@aaaraaawwrpQ     Ogrwwaj@@@@wjM88ccc|
     ,,,cccccccccccccccccccc8888888MMMMM@jrpjgrro8EQcr@.  oM.PJgEraajapr@M88cc|
     ,,ccccccccccccccccccc888888MMMMMM@@@jQQjgrOQQE            p grp8ogg8MM8cc|
    ,,ccccccccccccccccccc8888MMMMMMMM@@@jawGP  c                 g..  rgwMM88c|
    ,ccccccccccccccccc8888MMMMMMMMMM@@@jawrgGP                        crj@MM88|
    ,ccccccccccccccc888MMMMMMMMMMM@@@@jwOgQo                         Epaj@MM88|
   ,cccccccccccc88888Mja@@@@@@@@@@@jjjawOM                            ,waj@M88|
   ,cccccccc8888888MM@jwaajjjaoajjjjjaarPP                            OOpPjM88|
   cccccc88888888MMM@@aoorrwwrOrwwaaawro                                 JjM88|
   cccc88888888MMMMM@@jwpJJPggPOO8pwwrpp                               jpa@M88|
  ,cc888888888MMMMMM@jjwr.  @     .Epogp                                og@M88|
  cc888888888MMMMMM@jjapoPg        GjOE                                pww@M88|
  c88888888MMMMMM@aaawrgQr           ,.                                oQjMM88|
  c8888888M@@@@@jjaGpggP              w                                Oa@MM88|
  8MMMMM@j@@@@jjjwwo@wGc              a                               Jwj@MM88|
  MM@@jjrgwawwawpggOJ                                                .wj@@MM88|
  MM@@jjrgwawwawpggOJ                                                .wj@@MM88|
  8MMMMM@j@@@@jjjwwo@wGc              a                               Jwj@MM88|
  c8888888M@@@@@jjaGpggP              w                                Oa@MM88|
  c88888888MMMMMM@aaawrgQO           ,.                                oQjMM88|
  cc888888888MMMMMM@jjapoPg        GjOE                                pww@M88|
  ,cc888888888MMMMMM@jjwr.  @     .Epogp                                og@M88|
   cccc88888888MMMMM@@jwpJJPggPOO8pwwrpp                               rpa@M88|
   cccccc88888888MMM@@aoorrwwrOrwwaaawro                                 JjM88|
   ,cccccccc8888888MM@jwaajjjaoajjjjjaarPP                            OOpPjM88|
   ,cccccccccccc88888Mja@@@@@@@@@@@jjjawOM                            ,waj@M88|
    ,ccccccccccccccc888MMMMMMMMMMM@@@@jwOgQo                         Epaj@MM88|
    ,ccccccccccccccccc8888MMMMMMMMMM@@@jawrgGP                        crj@MM88|
    ,,ccccccccccccccccccc8888MMMMMMMM@@@jawGP  c                 g..  rgwMM88c|
     ,,ccccccccccccccccccc888888MMMMMM@@@jQQjgrOQQE            p grp8ogg8MM8cc|
     ,,,cccccccccccccccccccc8888888MMMMM@jrpjgrro8EQcr@.  oM.PJgEraajapr@M88cc|
      ,,cccccccccccccccccccccc88888888MMM@aaaraaawwrpQ     Ogrwwaj@@@@wjM88ccc|
      ,,,cccccccccccccccccccccc8888888888MM@@@@@jjawo@      Qwaj@@@@MMMM88cccc|
       ,,,ccccccccccccccccccccccc8888888888MMM@@@jjarP      @wj@@MMMMM8888cccc|
       ,,,,,cccccccccccccccccccccc88888888888MMM@@@wQQGa  MO8w@MMMMM88888ccccc|
        ,,,,,ccccccccccccccccccccccc8888888888MMMM@@aOrOJ .raj@MMM888888cccccc|
         ,,,,,cccccccccccccccccccccccc8888888888MMMM@jaroEpaj@MM8888888cccccc,|

*/

    #include <complex>
    #include <cstdio>
 
#if OMP
    #include <omp.h>
#endif

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
        const complex begin = center-span/2.0; // end = center+span/2.0;
        const int maxiter = 100000;

#if OMP
    // OpenMP > 2.0
    #pragma omp parallel for ordered schedule(dynamic)
#endif
        for(int pix=0; pix<num_pixels; ++pix)
        {
            const int x = pix%width, y = pix/width;

            complex p = begin + complex(
                x * span.real() / (width +1.0),
                y * span.imag() / (height+1.0));

            int n = MandelbrotCalculate(p, maxiter);
            if(n == maxiter) n = 0;

#if OMP
            #pragma omp ordered
            {
#endif
                char c = ' ';
                if(n > 0)
                {
                    static const char charset[] = ".,c8M@jawrpogOQEPGJ";
                    c = charset[n % (sizeof(charset)-1)];
                }
                std::putchar(c);
                if(x+1 == width) std::puts("|");
#if OMP
            }
#endif
        }

        return 0;
    }
