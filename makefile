CC=g++
CFLAGS=`pkg-config --cflags opencv` -O3 -fopenmp
LDFLAGS=`pkg-config --libs opencv` -fopenmp
SOURCES= makefile consts.h

all: contour

contour: ant.o colony.o contour.o
	@$(CC) -o $@ $^ $(LDFLAGS) -l fftw3

%.o: %.cpp $(SOURCES)
	@$(CC) -o $@ -c $< $(CFLAGS)

clean:
	@rm -rf *.o
