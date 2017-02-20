CPPFLAGS=-I/usr/local/cuda/include
CFLAGS=-std=gnu99
LDLIBS=-lOpenCL

all: main

run: main
	./main

clean:
	rm -rf *.o main
