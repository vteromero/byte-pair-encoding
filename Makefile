CC=g++
CFLAGS=-c -Wall -std=c++11

all: bpe

bpe: compress.o decompress.o bpe.o
	$(CC) compress.o decompress.o bpe.o -o bpe

bpe.o:
	$(CC) $(CFLAGS) bpe.cc

compress.o:
	$(CC) $(CFLAGS) compress.cc

decompress.o:
	$(CC) $(CFLAGS) decompress.cc

clean:
	rm -f *.o bpe