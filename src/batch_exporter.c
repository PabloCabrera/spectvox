#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sndfile.h>

#include "export_png.h"
#include "load_wav.h"
#include "spectrogram.h"

#define SPECTROGRAM_WINDOW_SIZE_DEFAULT 128

/* DATA TYPES */
struct program_options {
	char *input_filename;
	FILE *input_file;
	char *loading_error;
	int generate_png_fullsize;
	int generate_png_halfsize;
	int generate_raw_fullsize;
	int generate_raw_halfsize;
	unsigned spectrogram_window_size;
};


/* FUNCTION PROTOTYPES */
int main (int argc, char *args[]);
void nextstep (char *filename, struct program_options options);
void print_usage (char *bin_filename);
struct program_options get_options (int argc, char *args[]);
void trim_line (char *line);
char *replace_extension (char *filename, char *extension);


/* FUNCTIONS */
int main (int argc, char *args[]) {
	struct program_options options = get_options (argc, args);

	if (options.loading_error != NULL) {
		fprintf (stderr, "%s\n", options.loading_error);
		return 1;
	}

	FILE *file = options.input_file;
	char *line = malloc (512 * sizeof (char));
	size_t line_size = 512;
	while (getline (&line, &line_size, file) > 0) {
		trim_line (line);
		nextstep (line, options);
	}

	free (line);
	
	return 0;
}

void nextstep (char *filename, struct program_options options) {
	double *wav_data, *fullsize_spectrogram;
	unsigned long data_length = load_wav (filename, &wav_data);
	unsigned window = options.spectrogram_window_size;
	char *out_filename;
	
	fullsize_spectrogram = generate_smooth_spectrogram (wav_data, data_length, window);
	
	if (options.generate_png_fullsize) {
		out_filename = replace_extension (filename, ".png");
		export_png (fullsize_spectrogram, window, data_length/window, window, out_filename);
		free (out_filename);
	}
	if (options.generate_raw_fullsize) {
		out_filename = replace_extension (filename, ".raw");
		export_raw_spectrogram (fullsize_spectrogram, data_length, out_filename);
		free (out_filename);
	}

	if  (options.generate_png_halfsize || options.generate_raw_halfsize) {
		double *halfsize_spectrogram = reduce_spectrogram (fullsize_spectrogram, data_length, window);
		if  (options.generate_png_halfsize) {
			out_filename = replace_extension (filename, ".half.png");
			export_png (halfsize_spectrogram, window/2, data_length/(window*2), window/2, out_filename);
			free (out_filename);
		}
		if  (options.generate_png_halfsize) {
			out_filename = replace_extension (filename, ".half.raw");
			export_raw_spectrogram (fullsize_spectrogram, data_length/4, out_filename);
			free (out_filename);
		}
		free (halfsize_spectrogram);
	}

	free (fullsize_spectrogram);
	free (wav_data);
}

void print_usage (char *bin_filename) {
	printf ("%s [OPTIONS] input_file\n", bin_filename);
	printf ("OPTIONS:\n");
	("\t-p Generate PNG spectrogram images (Default type)");
	("\t-r Generate raw spectrogram data files");
	("\t-h Generate full size and half size spectrograms");
	("\t-H Generate only half size spectrograms");
}

struct program_options get_options (int argc, char *args[]) {
	struct program_options options;

	options.loading_error = NULL;

	if (argc < 1) {
		print_usage (args[0]);
		options.loading_error = "At least 1 argument expected";
		return options;
	}

	options.input_filename = args [argc -1];
	options.input_file = fopen (options.input_filename, "r");
	options.spectrogram_window_size = SPECTROGRAM_WINDOW_SIZE_DEFAULT;

	if (options.input_file == NULL) {
		options.loading_error = "Could not load input file.";
		return options;
	}

	int current_arg;
	int png = 0;
	int raw = 0;
	int full = 1;
	int half = 0;
	for (current_arg = 1; current_arg < argc -1; current_arg++) {
		png = png  || (index (args [current_arg], 'p') != NULL);
		raw = raw || (index (args [current_arg], 'r') != NULL); 
		half = half || (index (args [current_arg], 'h') != NULL) || (index (args [current_arg], 'H') != NULL);
		full = full && (index (args [current_arg], 'H') == NULL);
	}

	if (!raw && !png) {
		png = 1;
	}

	options.generate_png_fullsize = (full && png);
	options.generate_png_halfsize = (half && png);
	options.generate_raw_fullsize = (full && raw);
	options.generate_raw_halfsize = (half && raw);

	return options;
}

void trim_line (char *line) {
	int pos = 0;
	while (line[pos] != '\0') {
		if (line [pos] == '\r' || line [pos] == '\n') {
			line [pos] = '\0';
			return;
		}
		pos++;
	}
}

char *replace_extension (char *filename, char *extension) {
	char *last_dot = rindex (filename, '.');
	int base_length, length;
	if (last_dot == NULL) {
		base_length = strlen (filename);
		length = base_length + strlen (extension);
	} else {
		base_length = strlen (filename) - strlen (last_dot);
		length = base_length + strlen (extension);
	}

	char *output = malloc (sizeof (char) * (length + 1));
	strncpy (output, filename, base_length);
	strncpy (output + base_length, extension, strlen (extension) + 1);
	
	return output;
}

