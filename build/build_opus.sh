#!/bin/sh

# Detect architecture and set include/library paths
if [[ $(arch) == 'i386' ]]; then
  	echo "Intel Mac detected"
	IDIR="/usr/local/include"
	LDIR="/usr/local/lib"
elif [[ $(arch) == 'arm64' ]]; then
  	echo "M1/M2 Mac detected"
	IDIR="/opt/homebrew/include"
	LDIR="/opt/homebrew/lib"
else
	echo "Windows or other system detected"
	IDIR="/usr/include"
	LDIR="/usr/lib"
fi

# Compiler options
CC=gcc  # Use gcc for C files
CXX=g++ # Use g++ for C++ files
CFLAGS="-Wall"  # Add -g for debugging or -O2 for optimization

# Build Encoder
echo "Building encode..."
$CC $CFLAGS \
	-o encode encode.c \
	-I$IDIR -I$IDIR/opus \
	-L$LDIR \
	-lsndfile -lopus

# Build Decoder
echo "Building decode..."
if [ -f "decode.c" ]; then
    $CC $CFLAGS \
        -o decode decode.c \
        -I$IDIR -I$IDIR/opus \
        -L$LDIR \
        -lsndfile -lopus
else
    echo "decode.c not found, skipping decode build."
fi

echo "Build completed!"
