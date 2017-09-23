CC = g++
EXE = create_random_file get_histogram

.PHONY: all clean

all: $(EXE)

create_random_file: create_random_file.cc library.o
		$(CC) -o $@ $< library.o
	
get_histogram: get_histogram.cc library.o
		$(CC) -o $@ $< library.o

library.o: library.cc library.h
		$(CC) -o library.o -c library.cc

clean:
		rm -f $(EXE) library.o
