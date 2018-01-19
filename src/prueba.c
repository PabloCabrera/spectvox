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

double *generar_espectrograma (double data[], unsigned length, unsigned ventana) {
	fftw_plan plan;
	double *output = malloc (sizeof(double) * length);
	double *data_hanned = malloc (sizeof (double) * ventana);
	/* warning: el procesamiento se trunca a un multiplo de ventana */
	for (unsigned posicion = 0; posicion + ventana <= length; posicion += ventana) {
		hann (data + posicion, ventana, data_hanned);
		plan = fftw_plan_r2r_1d (ventana, data_hanned, output + posicion, FFTW_REDFT00, FFTW_ESTIMATE);
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

double *arreglar_espectrograma (double *input, unsigned long largo_data) {
	double *output = malloc (sizeof (double) * largo_data);
	int i;
	for (i=0; i < largo_data; i++) {
		output[i] = smooth (input[i]);
	}
	return output;
}


void mostrar_data (double data[], unsigned length) {
	for (unsigned n_muestra = 0; n_muestra < length; n_muestra++) {
		printf ("%f ", data [n_muestra]);
	}
	printf ("\n");
}

void mostrar_espectrograma (double data[], unsigned length, unsigned ventana) {
	char *signo, *separador;
	for (unsigned n_muestra = 0; n_muestra < length; n_muestra++) {
		signo = (data[n_muestra] >= 0)? "+": "";
		separador = ((n_muestra+1) % ventana == 0)? "\n": ", ";
		printf ("%e%s%ei%s", data [n_muestra], signo, data [n_muestra], separador);
	}
	printf ("\n");
}

int main (int argc, char *args[]) {
	double *data;
	unsigned long largo_data = load_wav (args[1], &data);
	unsigned ventana = 128;
	double *espectrograma, *arreglado;
	espectrograma = generar_espectrograma (data, largo_data, ventana);
	arreglado = arreglar_espectrograma (espectrograma, largo_data);
	exportar_png (arreglado, ventana, largo_data/ventana, ventana, "output/espectrograma.png");
	free (espectrograma);
}
