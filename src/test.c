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
	double *spectrogram, *tiny;
	spectrogram = generate_smooth_spectrogram (data, data_length, window);
	tiny = reduce_spectrogram (spectrogram, data_length, window);
	export_png (spectrogram, window, data_length/window, window, "output/spectrogram.png");
	export_png (tiny, window/2, data_length/(window*4), window/2, "output/tiny.png");
	free (spectrogram);
	free (tiny);
}
