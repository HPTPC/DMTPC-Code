CC=g++
LDFLAGS=`root-config --libs`
CFLAGS=-g `root-config --cflags` -O3


all: AsciiConverter BinaryConverter

AsciiConverter: AsciiConverter.cc
	$(CC) AsciiConverter.cc -o AsciiConverter $(CFLAGS) $(LDFLAGS)

BinaryConverter: BinaryConverter.cc
	$(CC) BinaryConverter.cc -o BinaryConverter $(CFLAGS) $(LDFLAGS)

