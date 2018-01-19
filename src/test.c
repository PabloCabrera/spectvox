#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <fftw3.h>
#include <png.h>
#include <sndfile.h>
#include "export_png.h"
#include "load_wav.h"

unsigned hann (double *data, unsigned length, double *output) {
	for (unsigned i = 0; i < length; i++) {
    	double multiplier = 0.5 * (1 - cos (2*M_PI*i/(length-1)));
    	output[i] = multiplier * data[i];
	}
	return length;
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

double smooth (double valor) {
	if (valor > 1) {
		return 1;
	} else {
		return (M_PI/2+asin(((2*valor-1))))/M_PI;
	}
}

double *smooth_spectrogram (double *input, unsigned long data_length) {
	double *output = malloc (sizeof (double) * data_length);
	int i;
	for (i=0; i < data_length; i++) {
		output[i] = smooth (input[i]);
	}
	return output;
}


void show_data (double data[], unsigned length) {
	for (unsigned n_sample = 0; n_sample < length; n_sample++) {
		printf ("%f ", data [n_sample]);
	}
	printf ("\n");
}

void show_spectrogram (double data[], unsigned length, unsigned window) {
	char *sign, *separator;
	for (unsigned n_sample = 0; n_sample < length; n_sample++) {
		sign = (data[n_sample] >= 0)? "+": "";
		separator = ((n_sample+1) % window == 0)? "\n": ", ";
		printf ("%e%s%ei%s", data [n_sample], sign, data [n_sample], separator);
	}
	printf ("\n");
}

int main (int argc, char *args[]) {
	double *data;
	unsigned long data_length = load_wav (args[1], &data);
	unsigned window = 128;
	double *spectrogram, *smoothed;
	spectrogram = generate_spectrogram (data, data_length, window);
	smoothed = smooth_spectrogram (spectrogram, data_length);
	export_png (smoothed, window, data_length/window, window, "output/spectrogram.png");
	free (spectrogram);
}
