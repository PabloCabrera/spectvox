#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <png.h>
#include <fftw3.h>
#include "export_png.h"

void export_png (
	double data[],
	unsigned window,
	unsigned length,
	unsigned height,
	char filename[]
) {
	FILE *file = fopen (filename, "wb");
	unsigned char *pixels = malloc (length * height * sizeof (unsigned char));
	for (unsigned x = 0; x < length; x++) {
		for (unsigned y = 0; y < height; y++) {
			int pos_data = x * window + y * (((float)height)/((float)window));
			//unsigned char value = (255 * data[x * window + y * (height/window)][0]);
			double value = (data[pos_data]);
			pixels [(height-y-1)*length + x] = (unsigned char)(255 * (value));
		}
	}
	write_png (pixels, length, height, file);

	free (pixels);
	fclose (file);
}


void write_png (unsigned char *data, unsigned int width, unsigned int height, FILE *file) {
	png_structp png_ptr = png_create_write_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr)
		fprintf (stderr, "Error, invalid png pointer");

	png_infop info_ptr = png_create_info_struct(png_ptr);

	if (!info_ptr) {
		png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
		fprintf (stderr, "Error invalid info pointer");
	}

	png_init_io (png_ptr, file);

	png_set_IHDR (
		png_ptr, info_ptr,
		width, height,
		8, // color depth
		PNG_COLOR_TYPE_GRAY,
		PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_DEFAULT,
		PNG_FILTER_TYPE_DEFAULT
	);

	png_write_info (png_ptr, info_ptr);
	
	png_byte *buf = (png_byte*) malloc (sizeof (png_byte) * width);
	int row, col;
	for (row = 0; row < height; row++) {
		for (col = 0; col < width; col++) {
			buf[col] = data[row*width + col];
		}
		png_write_row (png_ptr, buf);
	}

	free (buf);
	png_write_end (png_ptr, NULL);
	png_free_data (png_ptr, info_ptr, PNG_FREE_ALL, -1);
	png_destroy_write_struct (&png_ptr, (png_infopp) NULL);
}

