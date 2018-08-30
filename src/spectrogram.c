#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include <fftw3.h>

#include "spectrogram.h"

long hann (double *data, long length, double *output) {
	for (long i = 0; i < length; i++) {
    	double multiplier = 0.5 * (1 - cos (2*M_PI*i/(length-1)));
    	output[i] = multiplier * data[i];
	}
	return length;
}

double *generate_smooth_spectrogram (double data[], long length, long window, unsigned overlap) {
	double *spectrogram = generate_spectrogram (data, length, window, overlap);
	smooth_spectrogram (spectrogram, length*overlap/2);
	return spectrogram;
}

double *generate_spectrogram (double data[], long in_length, long window, unsigned overlap) {
	fftw_plan plan;
	long out_length = in_length/2;
	double *output = malloc (sizeof (double) * overlap * out_length);
	double *data_hanned = malloc (sizeof (double) * window);
	fftw_complex *buffer = malloc (sizeof (fftw_complex) * window);

	/* warning: el procesamiento se trunca a un multiplo de window */
	for (long position = 0; position + window/overlap < in_length; position += window/overlap) {
		hann (data + position, window, data_hanned);
		plan = fftw_plan_dft_r2c_1d (window, data_hanned, buffer, FFTW_ESTIMATE);
		fftw_execute (plan);
		long pos_out;
		for (pos_out=0; pos_out < window; pos_out++) {
			output [position*overlap/2+pos_out] = sqrt (pow (buffer[pos_out][0], 2) + pow (buffer[pos_out][1], 2));
		}
		fftw_destroy_plan (plan);
	}
	free (buffer);
	free (data_hanned);

	return output;
}

double *colorize_spectrogram (double *data, long length) {
	double *output = malloc (3 * length * sizeof (double));
	double r, g, b, val;
	long i;
	for (i = 0; i < length; i++) {
		val = data[i];
		b = (val <= (1.0/6))? 6*val: ((val <= (3.0/6))? 1: ((val <= (4.0/6))? 6*((4.0/6) - val): 0));
		g = (val <= 2.0/6)? 0: ((val <= 3.0/6)? 6*(val-2.0/6): ((val <= 5.0/6)? 1: 1-6*(val-5.0/6)));
		r = (val <= 4.0/6)? 0: ((val <= 5.0/6)? 6*(val - 4.0/6): 1);
		output [3*i] = r;
		output [3*i+1] = g;
		output [3*i+2] = b;
	}
	return output;
}

void export_raw_spectrogram (double data[], long length, char *filename) {
	FILE *file = fopen (filename, "w");
	fwrite (data, sizeof (double), length, file);
}

long import_raw_spectrogram (double **data, char *filename) {
	/* Max 4GiB */
	int stop = false;
	double *tmp_data = 0;
	long buffer_size = 0;
	long total_readed = 0;
	FILE *file = fopen (filename, "r");

	if (file == NULL) {
		return -1;
	}

	while (!stop) {
		if (total_readed + 1024 > buffer_size) {
			tmp_data = realloc (tmp_data, (buffer_size + 1024) * sizeof (double));
			buffer_size += 1024;
		}
		long readed = fread (tmp_data + total_readed, sizeof (double), 64, file);
		total_readed += readed;
		if (readed < 64) {
			stop = true;
		}
	}

	*data = tmp_data;
	return total_readed;
}

void smooth_spectrogram (double *input, long data_length) {
	int i;
		for (i=0; i < data_length; i++) {
			input[i] = smooth (input[i]);
		}
}
 
double smooth (double valor) {
	if (valor >= 1) {
		return 1;
	} else if (valor <= 0) {
		return 0;
	} else {
		return (M_PI/2+asin(((2*valor-1))))/M_PI;
	}
}
 
void smooth_spectrogram_blur (double *input, long data_length, int window) {
	int i, row, col, max_cols;
	double neigh[9];
	double *copy = malloc (data_length * sizeof (double));
	memcpy (copy, input, data_length * sizeof (double));
	max_cols = data_length/window;
	for (i=0; i < data_length; i++) {
			col = i / window;
			row = i % window;

			neigh[0] = (col>0 && row>0)? copy[i-window-1]: copy[i];
			neigh[1] = (col>0)? copy[i-window]: copy[i];
			neigh[2] = (col>0 && row<(window-1))? copy[i-window+1]: copy[i];

			neigh[3] = (row>0)? copy[i-1]: copy[i];
			neigh[4] = copy[i];
			neigh[5] = (row<window-1)? copy[i+1]: copy[i];

			neigh[6] = (col<max_cols && row>0)? copy[i+window-1]: copy[i];
			neigh[7] = (col<max_cols)? copy[i+window]: copy[i];
			neigh[8] = (col<max_cols && row<(window-1))? copy[i+window+1]: copy[i];

			input[i] = smooth_blur (neigh);
	}
	free (copy);
}
double smooth_blur (double *neigh) {
	int p;
	double sum=0;
	for (p=0; p<9; p++) {
		sum += neigh[p];
	}
	return smooth (sum/9);
}

