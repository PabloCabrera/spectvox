#include <stdlib.h>
#include <math.h>
#include <fftw3.h>

#include "spectrogram.h"

unsigned hann (double *data, unsigned length, double *output) {
	for (unsigned i = 0; i < length; i++) {
    	double multiplier = 0.5 * (1 - cos (2*M_PI*i/(length-1)));
    	output[i] = multiplier * data[i];
	}
	return length;
}

double *generate_smooth_spectrogram (double data[], unsigned length, unsigned window) {
	double *spectrogram = generate_spectrogram (data, length, window);
	smooth_spectrogram (spectrogram, length);
	return spectrogram;
}

double *generate_spectrogram (double data[], unsigned length, unsigned window) {
	fftw_plan plan;
	double *output = malloc (sizeof(double) * length);
	double *data_hanned = malloc (sizeof (double) * window);
	/* warning: el procesamiento se trunca a un multiplo de window */
	for (unsigned position = 0; position + window <= length; position += window) {
		hann (data + position, window, data_hanned);
		plan = fftw_plan_r2r_1d (window, data_hanned, output + position, FFTW_REDFT00, FFTW_ESTIMATE);
		fftw_execute (plan);
		fftw_destroy_plan (plan);
	}
	free (data_hanned);
	return output;
}

double *reduce_spectrogram (double data[], unsigned length, unsigned window) {
	double *output = malloc (sizeof(double) * length/4);
	int total_rows = length / (window * 2);
	int total_cols = window/2;
	fprintf (stderr, "Reducir espectrograma:\nAncho:%d  Alto: %d\n\n", total_cols, total_rows);
	int row, col, pos;
	for (row = 0; row < total_rows; row++) {
		for (col = 0; col < total_cols; col++) {
			pos = row * total_cols + col;
			output [pos] = data [window * 2*row + 2*col];
		}
	}
	return output;
}

void export_raw_spectrogram (double data[], unsigned length, char *filename) {
	FILE *file = fopen (filename, "w");
	fwrite (data, sizeof (double),  length, file);
}

void smooth_spectrogram (double *input, unsigned long data_length) {
	int i;
	for (i=0; i < data_length; i++) {
		input[i] = smooth (input[i]);
	}
}

double smooth (double valor) {
	if (valor > 1) {
		return 1;
	} else {
		return (M_PI/2+asin(((2*valor-1))))/M_PI;
	}
}
