#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
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
	long spectrogram_window_size;
	bool generate_png;
	bool generate_raw;
	bool color;
};


/* FUNCTION PROTOTYPES */
int main (int argc, char *args[]);
void nextstep (char *filename, struct program_options options, bool color);
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
		if (strlen (line) > 0) {
			nextstep (line, options, options.color);
		}
	}

	free (line);
	
	return 0;
}

void nextstep (char *filename, struct program_options options, bool color) {
	double *wav_data, *spectrogram;
	long data_length = load_wav (filename, &wav_data);
	long window = options.spectrogram_window_size;
	char *out_filename;
	
	spectrogram = generate_smooth_spectrogram (wav_data, data_length, window);
	if (options.color) {
		double *grayscale_spectrogram = spectrogram;
		spectrogram = colorize_spectrogram (grayscale_spectrogram, data_length);
		free (grayscale_spectrogram);
	}
	
	if (options.generate_png) {
		out_filename = replace_extension (filename, ".png");
		if (options.color) {
			export_color_png (spectrogram, window, data_length/window, window, out_filename);
		} else {
			export_grayscale_png (spectrogram, window, data_length/window, window, out_filename);
		}
		free (out_filename);
	}
	if (options.generate_raw) {
		out_filename = replace_extension (filename, ".raw");
		if (options.color) {
			export_raw_spectrogram (spectrogram, 3*data_length, out_filename);
		} else {
			export_raw_spectrogram (spectrogram, data_length, out_filename);
		}
		free (out_filename);
	}


	free (spectrogram);
	free (wav_data);
}

void print_usage (char *bin_filename) {
	printf ("%s [OPTIONS] input_file\n", bin_filename);
	printf ("OPTIONS:\n");
	printf ("\t-p Generate PNG spectrogram images (Default type)\n");
	printf ("\t-r Generate raw spectrogram data files\n");
	printf ("\t-c Generate colored spectrogram\n");
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
	for (current_arg = 1; current_arg < argc -1; current_arg++) {
		options.generate_png = (index (args [current_arg], 'p') != NULL);
		options.generate_raw = (index (args [current_arg], 'r') != NULL); 
		options.color = (index (args [current_arg], 'c') != NULL); 
	}

	if (!options.generate_raw && !options.generate_png) {
		options.generate_png = true;
		options.generate_raw = false;
	}

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

