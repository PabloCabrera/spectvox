#include <png.h>
#include <stdio.h>
#include <stdlib.h>
#include "pngtest.h"

int main (int argc, char **args) {
	FILE *file = get_output_file (argc, args);
	double data[1024];
	randomize_image (data, 1024);
	diagonal_line (data, 32);
	write_png (data, 32, 32, file);
	fclose (file);
}

FILE *get_output_file (int argc, char **args) {
	char *output_filename;

	if (argc > 1) {
		output_filename = args[1];
	} else {
		output_filename = "output/pngout.png";
	}
	
	return fopen (output_filename, "w");
}

void randomize_image (double *data, unsigned int length) {
	int count;
	for (count = 0; count < length; count++) {
		data[count] = 1.f / (rand() % 255);
	}
}

void diagonal_line (double *data, int width) {
	int x;
	for (x = 0; x < width; x++) {
		data[x*width+x] = 1;
	}
}

void write_png (double *data, unsigned int width, unsigned int height, FILE *file) {
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
			buf[col] = 255 * data[row*width + col];
		}
		png_write_row (png_ptr, buf);
	}

	free (buf);
	png_write_end (png_ptr, NULL);
	png_free_data (png_ptr, info_ptr, PNG_FREE_ALL, -1);
	png_destroy_write_struct (&png_ptr, (png_infopp) NULL);
}

