// Nebulabrot / Buddhabrot generator.
// Brought to you by Wikipedia...
// Written by User:Evercat
//
// Released under the GNU Free Documentation License
// or the GNU Public License, whichever you prefer:
// November 23, 2004
//
// This code is lame and confusing. I apologise.
// As I like to point out, my C is self-taught.
//
// Note: some folk mention possible improvements on the talk page:
// http://en.wikipedia.org/wiki/User_talk:Evercat/Buddhabrot.c
 
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
 
#define OUTFILE "buddhabrot"
 
////////////////////////////////////////////////////////////////////////////////////
 
#define WIDTH 1024
#define HEIGHT 768
 
#define CENTRE_X -0.451					// For full set try -0.451 by 0
#define CENTRE_Y 0
#define ZOOM 310					// Try 310
 
#define SOURCE_COLUMNS (WIDTH * 10)			// These lines control the number of source values iterated.
#define SOURCE_ROWS (HEIGHT * 10)			// Try WIDTH and HEIGHT * 10
 
#define UPDATETICK 100					// Report status every nth column reached
 
#define RED_MULTIPLIER 0.09				// Multiply the number of hits in a pixel by these values
#define GREEN_MULTIPLIER 0.11				// (needs to be lower the greater the number of source
#define BLUE_MULTIPLIER 0.18				// values and iterations, else image will be too bright)
 
#define COLOUR_OFFSET -230				// This value is added to the number of hits before a pixel is drawn.
							// It needs to be lower the more source columns/rows there are.
#define RED_ITERATIONS_LOWER 0
#define RED_ITERATIONS_UPPER 1000
#define GREEN_ITERATIONS_LOWER 0
#define GREEN_ITERATIONS_UPPER 1000
#define BLUE_ITERATIONS_LOWER 0
#define BLUE_ITERATIONS_UPPER 1000
 
#define SOURCE_LEFT_X -2.102613
#define SOURCE_RIGHT_X 1.200613
#define SOURCE_TOP_Y -1.237710
#define SOURCE_BOTTOM_Y 1.239710
 
#undef RANDOM_SOURCE
 
//#define OPTIMISE		// Don't bother iterating some values obviously in the set.
 
#define reduce(foo) (foo)	// Macro to reduce colors, can use sqrt(n), log(n), etc, or just (n)
 
////////////////////////////////////////////////////////////////////////////////////
 
void drawpath(double x, double y, double target_startx, double target_starty, double pixel_width);
double rnd (void);
void drawbmp (char * filename);
 
////////////////////////////////////////////////////////////////////////////////////
 
long long redcount[WIDTH][HEIGHT];
long long greencount[WIDTH][HEIGHT];
long long bluecount[WIDTH][HEIGHT];
 
int iterations;
 
double red_multiplier = RED_MULTIPLIER;
double green_multiplier = GREEN_MULTIPLIER;
double blue_multiplier = BLUE_MULTIPLIER;
 
////////////////////////////////////////////////////////////////////////////////////
 
int main (int argc, char * argv[]) {
 
	int i, j, n;				// General purpose counters
	double x, y;				// Source coordinates of particle being tracked
	int source_column, source_row;		// Source grid location
	double r, s, nextr, nexts;		// Values as particle is iterated through the Mandelbrot function
	double x_jump, y_jump;			// Distances between particles in the source grid
	double target_startx, target_starty;	// Top-left coordinates of drawn area
	double target_endx, target_endy;	// Bottom-right coordinates of drawn area
	double pixel_width;			// Actual distance represented by a pixel in the drawn area
 
	char filename[200];
 
    // cleanup
    (void)argc;
    (void)argv;
    (void)target_endy;
 
	for (i = 0; i < WIDTH; i++)
	{
		for (j = 0; j < HEIGHT; j++)
		{
			redcount[i][j] = 0;
			greencount[i][j] = 0;
			bluecount[i][j] = 0;
		}
	}
 
	target_startx = CENTRE_X - ((double) WIDTH / (ZOOM * 2));
	target_endx = CENTRE_X + ((double) WIDTH / (ZOOM * 2));
 
	target_starty = CENTRE_Y - ((double) HEIGHT / (ZOOM * 2));
	target_endy = CENTRE_Y + ((double) HEIGHT / (ZOOM * 2));
 
	pixel_width = (target_endx - target_startx) / WIDTH;
 
	x_jump = ((double) SOURCE_RIGHT_X - SOURCE_LEFT_X) / SOURCE_COLUMNS;
	y_jump = ((double) SOURCE_BOTTOM_Y - SOURCE_TOP_Y) / SOURCE_ROWS;
 
	iterations = RED_ITERATIONS_UPPER;
	if (GREEN_ITERATIONS_UPPER > iterations) iterations = GREEN_ITERATIONS_UPPER;
	if (BLUE_ITERATIONS_UPPER > iterations) iterations = BLUE_ITERATIONS_UPPER;
 
	// Main bit...
 
	x = SOURCE_LEFT_X;
	for (source_column = 0; source_column < SOURCE_COLUMNS; source_column++, x += x_jump)
	{
		y = SOURCE_TOP_Y;
		for (source_row = 0; source_row < SOURCE_ROWS; source_row++, y += y_jump)
		{
 
#ifdef OPTIMISE
			if 
			(
			   (x >  -1.2 && x <=  -1.1 && y >  -0.1 && y < 0.1)
			|| (x >  -1.1 && x <=  -0.9 && y >  -0.2 && y < 0.2)
			|| (x >  -0.9 && x <=  -0.8 && y >  -0.1 && y < 0.1)
			|| (x > -0.69 && x <= -0.61 && y >  -0.2 && y < 0.2)
			|| (x > -0.61 && x <=  -0.5 && y > -0.37 && y < 0.37)
			|| (x >  -0.5 && x <= -0.39 && y > -0.48 && y < 0.48)
			|| (x > -0.39 && x <=  0.14 && y > -0.55 && y < 0.55)
			|| (x >  0.14 && x <   0.29 && y > -0.42 && y < -0.07)
			|| (x >  0.14 && x <   0.29 && y >  0.07 && y < 0.42)
			) continue;
#endif
			r = 0; s = 0;
			for (n = 0; n <= iterations; n++)
			{
				nextr = ((r * r) - (s * s)) + x;
 				nexts = (2 * r * s) + y;
				r = nextr;
				s = nexts;
				if (n == iterations)
				{
					// drawpath(x, y, target_startx, target_starty, pixel_width);
					break;
				} else if ((r * r) + (s * s) > 4) {
					drawpath(x, y, target_startx, target_starty, pixel_width);
					break;
				}
			}
 
		}
 
		if (source_column % UPDATETICK == 0)
		{
			printf("Reached source column: %d of %d\n", source_column, SOURCE_COLUMNS);
		}
 
	}
 
	sprintf(filename, "%s r %d g %d b %d spp %d.bmp", OUTFILE, RED_ITERATIONS_UPPER, GREEN_ITERATIONS_UPPER, BLUE_ITERATIONS_UPPER, (SOURCE_COLUMNS / WIDTH) * (SOURCE_ROWS / HEIGHT));
	drawbmp(filename);
 
	printf("Done.\n");
	return 0;
}
 
 
void drawpath (double x, double y, double target_startx, double target_starty, double pixel_width)
{
	double r, s, nextr, nexts;
	int n;
	int xpixel, ypixel;
 
	r = 0; s = 0;
	for (n = 0; n <= iterations; n++)
	{
		nextr = ((r * r) - (s * s)) + x;
		nexts = (2 * r * s) + y;
		r = nextr;
		s = nexts;
 
		if ((r * r) + (s * s) > 4) return;
 
		xpixel = (r - target_startx) / pixel_width;
		ypixel = (s - target_starty) / pixel_width;
		if (xpixel > 0 && xpixel < WIDTH && ypixel > 0 && ypixel < HEIGHT)
		{
			if (n >= RED_ITERATIONS_LOWER && n <= RED_ITERATIONS_UPPER)
			{
				redcount[xpixel][ypixel] += 1;
			}
			if (n >= GREEN_ITERATIONS_LOWER && n <= GREEN_ITERATIONS_UPPER)
			{
				greencount[xpixel][ypixel] += 1;
			}
			if (n >= BLUE_ITERATIONS_LOWER && n <= BLUE_ITERATIONS_UPPER)
			{
				bluecount[xpixel][ypixel] += 1;
			}
		}
 
	}
	return;
}
 
 
void drawbmp (char * filename) {
    unsigned int headers[13];
    FILE * outfile;
    int extrabytes;
    int paddedsize;
    int x; int y; int n;
    int red, green, blue;
     
    extrabytes = 4 - ((WIDTH * 3) % 4);                 // How many bytes of padding to add to each
                                                        // horizontal line - the size of which must
                                                        // be a multiple of 4 bytes.
    if (extrabytes == 4)
       extrabytes = 0;
     
    paddedsize = ((WIDTH * 3) + extrabytes) * HEIGHT;
     
    // Headers...
    // Note that the "BM" identifier in bytes 0 and 1 is NOT included in these "headers".
     
    headers[0]  = paddedsize + 54;      // bfSize (whole file size)
    headers[1]  = 0;                    // bfReserved (both)
    headers[2]  = 54;                   // bfOffbits
    headers[3]  = 40;                   // biSize
    headers[4]  = WIDTH;  // biWidth
    headers[5]  = HEIGHT; // biHeight
     
    // Would have biPlanes and biBitCount in position 6, but they're shorts.
    // It's easier to write them out separately (see below) than pretend
    // they're a single int, especially with endian issues...
     
    headers[7]  = 0;                    // biCompression
    headers[8]  = paddedsize;           // biSizeImage
    headers[9]  = 0;                    // biXPelsPerMeter
    headers[10] = 0;                    // biYPelsPerMeter
    headers[11] = 0;                    // biClrUsed
    headers[12] = 0;                    // biClrImportant
     
    outfile = fopen(filename, "wb");
     
    //
    // Headers begin...
    // When printing ints and shorts, we write out 1 character at a time to avoid endian issues.
    //
     
    fprintf(outfile, "BM");
     
    for (n = 0; n <= 5; n++)
    {
       fprintf(outfile, "%c", headers[n] & 0x000000FF);
       fprintf(outfile, "%c", (headers[n] & 0x0000FF00) >> 8);
       fprintf(outfile, "%c", (headers[n] & 0x00FF0000) >> 16);
       fprintf(outfile, "%c", (headers[n] & (unsigned int) 0xFF000000) >> 24);
    }
     
    // These next 4 characters are for the biPlanes and biBitCount fields.
     
    fprintf(outfile, "%c", 1);
    fprintf(outfile, "%c", 0);
    fprintf(outfile, "%c", 24);
    fprintf(outfile, "%c", 0);
     
    for (n = 7; n <= 12; n++)
    {
       fprintf(outfile, "%c", headers[n] & 0x000000FF);
       fprintf(outfile, "%c", (headers[n] & 0x0000FF00) >> 8);
       fprintf(outfile, "%c", (headers[n] & 0x00FF0000) >> 16);
       fprintf(outfile, "%c", (headers[n] & (unsigned int) 0xFF000000) >> 24);
    }
     
    //
    // Headers done, now write the data...
    //
     
    for (y = HEIGHT - 1; y >= 0; y--)     // BMP image format is written from bottom to top...
    {
       for (x = 0; x <= WIDTH - 1; x++)
       {
     
          red = reduce(redcount[x][y] + COLOUR_OFFSET) * red_multiplier;
          green = reduce(greencount[x][y] + COLOUR_OFFSET) * green_multiplier;
          blue = reduce(bluecount[x][y] + COLOUR_OFFSET) * blue_multiplier;
     
          if (red > 255) red = 255; if (red < 0) red = 0;
          if (green > 255) green = 255; if (green < 0) green = 0;
          if (blue > 255) blue = 255; if (blue < 0) blue = 0;
     
          // Also, it's written in (b,g,r) format...
     
          fprintf(outfile, "%c", blue);
          fprintf(outfile, "%c", green);
          fprintf(outfile, "%c", red);
       }
       if (extrabytes)      // See above - BMP lines must be of lengths divisible by 4.
       {
          for (n = 1; n <= extrabytes; n++)
          {
             fprintf(outfile, "%c", 0);
          }
       }
    }
     
    fclose(outfile);
    return;
}
