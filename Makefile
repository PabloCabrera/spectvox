FFTW_FLAGS=-lfftw3
MATH_FLAGS=-lm
PNG_FLAGS=`libpng-config --cflags --ldflags`
SND_FLAGS=-lsndfile
ALL_FLAGS=${PNG_FLAGS} ${MATH_FLAGS} ${FFTW_FLAGS} ${SND_FLAGS}

all: bin/prueba bin/pngtest bin/wavtest

bin/prueba: src/prueba.c src/prueba.h src/exportar_png.c src/exportar_png.h src/load_wav.h src/load_wav.c
	gcc -std=gnu99 src/prueba.c src/exportar_png.c src/load_wav.c ${ALL_FLAGS} -o bin/prueba

src/prueba.h: src/prueba.c
	makeheaders src/prueba.c

src/exportar_png.h: src/exportar_png.c
	makeheaders src/exportar_png.c

bin/pngtest: src/pngtest.c src/pngtest.h
	gcc -std=gnu99 src/pngtest.c ${PNG_FLAGS} -o bin/pngtest

src/pngtest.h: src/pngtest.c
	makeheaders src/pngtest.c

bin/wavtest: src/load_wav.c src/load_wav.h src/wavtest.c
	gcc -std=gnu99 ${SND_FLAGS} src/wavtest.c src/load_wav.c -o bin/wavtest

src/load_wav.h: src/load_wav.c
	makeheaders src/load_wav.c
