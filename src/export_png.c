#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <png.h>

#include "export_png.h"

void export_grayscale_png (
	double data[],
	long window,
	long width,
	long height,
	char filename[]
) {
	FILE *file = fopen (filename, "wb");
	unsigned char *pixels = malloc (width * height * sizeof (unsigned char));
	for (long x = 0; x < width; x++) {
		for (long y = 0; y < height; y++) {
			long pos_data = x * window + y * (((float)height)/((float)window));
			//unsigned char value = (255 * data[x * window + y * (height/window)][0]);
			double value = (data[pos_data]);
			pixels [(height-y-1)*width + x] = (unsigned char)(255 * (value));
		}
	}
	write_png (pixels, width, height, file, false);

	free (pixels);
	fclose (file);
}

void export_color_png (
	double data[],
	long window,
	long width,
	long height,
	char filename[]
) {
	FILE *file = fopen (filename, "wb");
	unsigned char *pixels = malloc (3 * width * height * sizeof (unsigned char));
	for (long x = 0; x < width; x++) {
		for (long y = 0; y < height; y++) {
			int pos_data = x * window + y * (((float)height)/((float)window));
			//unsigned char value = (255 * data[x * window + y * (height/window)][0]);
			double r = (data[3*pos_data]);
			double g = (data[3*pos_data+1]);
			double b = (data[3*pos_data+2]);
			pixels [3*((height-y-1)*width + x)] = (unsigned char)(255 * (r));
			pixels [3*((height-y-1)*width + x)+1] = (unsigned char)(255 * (g));
			pixels [3*((height-y-1)*width + x)+2] = (unsigned char)(255 * (b));
		}
	}
	write_png (pixels, width, height, file, true);

	free (pixels);
	fclose (file);
}

void write_png (unsigned char *data, long width, long height, FILE *file, unsigned color) {
	png_structp png_ptr = png_create_write_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr)
		fprintf (stderr, "Error, invalid png pointer");

	png_infop info_ptr = png_create_info_struct(png_ptr);

	if (!info_ptr) {
		png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
		fprintf (stderr, "Error invalid info pointer");
	}

	png_init_io (png_ptr, file);
	int colortype = color? PNG_COLOR_TYPE_RGB: PNG_COLOR_TYPE_GRAY;

	png_set_IHDR (
		png_ptr, info_ptr,
		width, height,
		8, // color depth
		colortype,
		PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_DEFAULT,
		PNG_FILTER_TYPE_DEFAULT
	);

	png_write_info (png_ptr, info_ptr);
	
	if (color) {
		// Allocate memory for one row (3 bytes per pixel - RGB)
		png_byte *buf = malloc(4 * width * sizeof(png_byte));

		// Write image data
		long col, row;
		for (row=0 ; row < height; row++) {
			for (col=0 ; col < width; col++) {
				buf[3*col] = data[3*(row*width + col)];
				buf[3*col + 1] = data[3*(row*width + col)+1];
				buf[3*col + 2] = data[3*(row*width + col)+2];
			}
			png_write_row(png_ptr, buf);
		}
		free (buf);

	} else {
		png_byte *buf = (png_byte*) malloc (sizeof (png_byte) * width);
		int row, col;
		for (row = 0; row < height; row++) {
			for (col = 0; col < width; col++) {
				buf[col] = data[row*width + col];
			}
			png_write_row (png_ptr, buf);
		}
		free (buf);
	}

	png_write_end (png_ptr, NULL);
	png_free_data (png_ptr, info_ptr, PNG_FREE_ALL, -1);
	png_destroy_write_struct (&png_ptr, (png_infopp) NULL);
}

