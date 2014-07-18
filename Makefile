CC=g++
CFLAGS=-c -Wall -std=c++11

all: bpe

bpe: compress.o decompress.o endian.o bpe.o
	$(CC) compress.o decompress.o endian.o bpe.o -o bpe

bpe.o:
	$(CC) $(CFLAGS) bpe.cc

compress.o:
	$(CC) $(CFLAGS) compress.cc

decompress.o:
	$(CC) $(CFLAGS) decompress.cc

endian.o:
	$(CC) $(CFLAGS) endian.cc

clean:
	rm -f *.o bpe
