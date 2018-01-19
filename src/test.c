#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "export_png.h"
#include "load_wav.h"
#include "spectrogram.h"

int main (int argc, char *args[]) {
	double *data;
	unsigned long data_length = load_wav (args[1], &data);
	unsigned window = 128;
	double *spectrogram, *smoothed;
	spectrogram = generate_smooth_spectrogram (data, data_length, window);
	export_png (smoothed, window, data_length/window, window, "output/spectrogram.png");
	free (spectrogram);
}
