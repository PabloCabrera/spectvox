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
	double *spectrogram, *thumbnail;
	spectrogram = generate_smooth_spectrogram (data, data_length, window);
	thumbnail = reduce_spectrogram (spectrogram, data_length, window);
	export_png (spectrogram, window, data_length/window, window, "output/spectrogram.png");
	export_png (thumbnail, window/2, data_length/(window*2), window/2, "output/thumbnail.png");
	export_raw_spectrogram (spectrogram, data_length, "output/raw_spectrogram");
	export_raw_spectrogram (thumbnail, data_length/4, "output/raw_thumbnail");
	free (spectrogram);
	free (thumbnail);
}
