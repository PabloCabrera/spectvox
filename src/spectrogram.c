#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
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
	smooth_spectrogram (spectrogram, length*overlap);
	return spectrogram;
}

double *generate_spectrogram (double data[], long length, long window, unsigned overlap) {
	fftw_plan plan;
	double *output = malloc (sizeof(double) * overlap * length);
	double *data_hanned = malloc (sizeof (double) * window);
	/* warning: el procesamiento se trunca a un multiplo de window */
	for (long position = 0; position + window/overlap <= length; position += window/overlap) {
		hann (data + position, window, data_hanned);
		plan = fftw_plan_r2r_1d (window, data_hanned, output + overlap*position, FFTW_REDFT00, FFTW_ESTIMATE);
		fftw_execute (plan);
		fftw_destroy_plan (plan);
	}
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

