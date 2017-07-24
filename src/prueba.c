#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <fftw3.h>
#include <png.h>
#include <sndfile.h>
#include "exportar_png.h"
#include "load_wav.h"

unsigned hann (double *data, unsigned length, double *output) {
	for (unsigned i = 0; i < length; i++) {
    	double multiplier = 0.5 * (1 - cos (2*M_PI*i/(length-1)));
    	output[i] = multiplier * data[i];
	}
	return length;
}

fftw_complex *generar_espectrograma (double data[], unsigned length, unsigned ventana) {
	fftw_plan plan;
	fftw_complex *output = fftw_alloc_complex (length);
	double *data_hanned = malloc (sizeof (double) * ventana);
	/* warning: el procesamiento se trunca a un multiplo de ventana */
	for (unsigned posicion = 0; posicion + ventana <= length; posicion += ventana) {
		hann (data + posicion, ventana, data_hanned);
		plan = fftw_plan_dft_r2c_1d (ventana, data_hanned, output + posicion, FFTW_ESTIMATE);
		fftw_execute (plan);
		fftw_destroy_plan (plan);
	}
	free (data_hanned);
	return output;
}


void mostrar_data (double data[], unsigned length) {
	for (unsigned n_muestra = 0; n_muestra < length; n_muestra++) {
		printf ("%f ", data [n_muestra]);
	}
	printf ("\n");
}

void mostrar_espectrograma (fftw_complex data[], unsigned length, unsigned ventana) {
	char *signo, *separador;
	for (unsigned n_muestra = 0; n_muestra < length; n_muestra++) {
		signo = (data[n_muestra][1] >= 0)? "+": "";
		separador = ((n_muestra+1) % ventana == 0)? "\n": ", ";
		printf ("%e%s%ei%s", data [n_muestra][0], signo, data [n_muestra][0], separador);
	}
	printf ("\n");
}

int main (int argc, char *args[]) {
	double *data;
	unsigned long largo_data = load_wav (args[1], &data);
	unsigned ventana = 256;
	fftw_complex *espectrograma;
	espectrograma = generar_espectrograma (data, largo_data, ventana);
	exportar_png (espectrograma, ventana, largo_data/ventana, ventana, "output/espectrograma.png");
	free (espectrograma);
}
