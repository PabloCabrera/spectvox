/* This file was automatically generated.  Do not edit! */
double smooth(double valor);
void export_raw_spectrogram(double data[],unsigned length,char *filename);
double *reduce_spectrogram(double data[],unsigned length,unsigned window);
void smooth_spectrogram(double *input,unsigned long data_length);
double *generate_spectrogram(double data[],unsigned length,unsigned window);
double *generate_smooth_spectrogram(double data[],unsigned length,unsigned window);
unsigned hann(double *data,unsigned length,double *output);
