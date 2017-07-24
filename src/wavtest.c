#include <stdio.h>
#include <stdlib.h>
#include "load_wav.h"


int main (int argc, char **args) {
	double *buffer;
	unsigned long num_samples;
	if (argc > 0) {
		num_samples = load_wav (args[1], &buffer);
	} else {
		fprintf (stderr, "Uso: wavtest archivo.wav");
	}

	/* Do stuff */
	free (buffer);
}
