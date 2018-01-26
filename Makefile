FFTW_FLAGS=-lfftw3
MATH_FLAGS=-lm
PNG_FLAGS=`libpng-config --cflags --ldflags`
SND_FLAGS=-lsndfile
ALL_FLAGS=${PNG_FLAGS} ${MATH_FLAGS} ${FFTW_FLAGS} ${SND_FLAGS}

all: bin/test bin/pngtest bin/wavtest bin/import_test bin/spectvox

bin/test: src/test.c src/test.h src/export_png.c src/export_png.h src/load_wav.h src/load_wav.c src/spectrogram.c src/spectrogram.h
	gcc -std=gnu99 src/test.c src/export_png.c src/load_wav.c src/spectrogram.c ${ALL_FLAGS} -o bin/test

bin/import_test: src/import_test.c src/export_png.c src/export_png.h src/spectrogram.c src/spectrogram.h
	gcc -std=gnu99 src/import_test.c src/export_png.c src/spectrogram.c ${ALL_FLAGS} -o bin/import_test

bin/spectvox: src/batch_exporter.c src/export_png.c src/export_png.h src/load_wav.h src/load_wav.c src/spectrogram.c src/spectrogram.h
	gcc -std=gnu99 src/batch_exporter.c src/export_png.c src/load_wav.c src/spectrogram.c ${ALL_FLAGS} -o bin/spectvox

src/test.h: src/test.c
	makeheaders src/test.c

src/export_png.h: src/export_png.c
	makeheaders src/export_png.c

src/spectrogram.h: src/spectrogram.c
	makeheaders src/spectrogram.c

bin/pngtest: src/pngtest.c src/pngtest.h
	gcc -std=gnu99 src/pngtest.c ${PNG_FLAGS} -o bin/pngtest

src/pngtest.h: src/pngtest.c
	makeheaders src/pngtest.c

bin/wavtest: src/load_wav.c src/load_wav.h src/wavtest.c
	gcc -std=gnu99 ${SND_FLAGS} src/wavtest.c src/load_wav.c -o bin/wavtest

src/load_wav.h: src/load_wav.c
	makeheaders src/load_wav.c
