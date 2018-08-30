/* This file was automatically generated.  Do not edit! */
double smooth_blur(double *neigh);
void smooth_spectrogram_blur(double *input,long data_length,int window);
double smooth(double valor);
long import_raw_spectrogram(double **data,char *filename);
void export_raw_spectrogram(double data[],long length,char *filename);
double *colorize_spectrogram(double *data,long length);
void smooth_spectrogram(double *input,long data_length);
double *generate_spectrogram(double data[],long in_length,long window,unsigned overlap);
double *generate_smooth_spectrogram(double data[],long length,long window,unsigned overlap);
long hann(double *data,long length,double *output);
