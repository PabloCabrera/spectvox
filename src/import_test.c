#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "export_png.h"
#include "load_wav.h"
#include "spectrogram.h"

int main (int argc, char *args[]) {
	double *data;
	unsigned window = 128;
	double *spectrogram;
	fprintf (stderr, "Cargar datos en bruto\n");
	unsigned long data_length = import_raw_spectrogram (&spectrogram, args[1]);
	fprintf (stderr, "Leídos %ld pixeles\n", data_length);
	fprintf (stderr, "Exportar imagen\n");
	export_png (spectrogram, window, data_length/window, window, "output/imported.png");
	fprintf (stderr, "OK\n");
	free (spectrogram);
}
