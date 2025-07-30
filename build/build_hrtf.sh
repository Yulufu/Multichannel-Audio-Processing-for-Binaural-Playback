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
CFLAGS="-Wall -O2"  # Enable optimization and warnings

# Build HRTF Processor
echo "Building HRTF Processor..."
if [ -f "hrtf.c" ] && [ -f "main.c" ]; then
    $CC $CFLAGS \
        -o hrtf_processor main.c hrtf.c \
        -I$IDIR \
        -L$LDIR \
        -lsndfile -lm
else
    echo "Error: hrtf.c or main.c not found. Build aborted."
    exit 1
fi

echo "Build completed successfully!"
