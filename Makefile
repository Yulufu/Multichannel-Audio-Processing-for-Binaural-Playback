# Makefile for Multichannel Audio Processing for Binaural Playback

# Detect OS and architecture
UNAME_S := $(shell uname -s)
UNAME_M := $(shell uname -m)

# Default paths
IDIR=/usr/include
LDIR=/usr/lib

# macOS specific paths
ifeq ($(UNAME_S),Darwin)
    ifeq ($(UNAME_M),arm64)
        # Apple Silicon
        IDIR=/opt/homebrew/include
        LDIR=/opt/homebrew/lib
    else
        # Intel Mac
        IDIR=/usr/local/include
        LDIR=/usr/local/lib
    endif
endif

# Compiler settings
CC=gcc
CFLAGS=-Wall -O2 -I$(IDIR)
LDFLAGS=-L$(LDIR)
LIBS_OPUS=-lsndfile -lopus
LIBS_HRTF=-lsndfile -lm

# Directories
SRC_OPUS=src/part1-opus
SRC_HRTF=src/part2-hrtf
BUILD_DIR=build/bin

# Targets
all: directories opus hrtf

directories:
	@mkdir -p $(BUILD_DIR)

opus: $(BUILD_DIR)/encode $(BUILD_DIR)/decode

hrtf: $(BUILD_DIR)/hrtf_processor

# Part 1: Opus Encoder
$(BUILD_DIR)/encode: $(SRC_OPUS)/encode.c $(SRC_OPUS)/common.h
	$(CC) $(CFLAGS) -o $@ $(SRC_OPUS)/encode.c $(LDFLAGS) $(LIBS_OPUS)

# Part 1: Opus Decoder
$(BUILD_DIR)/decode: $(SRC_OPUS)/decode.c $(SRC_OPUS)/common.h
	$(CC) $(CFLAGS) -o $@ $(SRC_OPUS)/decode.c $(LDFLAGS) $(LIBS_OPUS)

# Part 2: HRTF Processor
$(BUILD_DIR)/hrtf_processor: $(SRC_HRTF)/main.c $(SRC_HRTF)/hrtf.c $(SRC_HRTF)/hrtf.h
	$(CC) $(CFLAGS) -o $@ $(SRC_HRTF)/main.c $(SRC_HRTF)/hrtf.c $(LDFLAGS) $(LIBS_HRTF)

# Install binaries to system
install: all
	@echo "Installing binaries to /usr/local/bin..."
	@sudo cp $(BUILD_DIR)/* /usr/local/bin/
	@echo "Installation complete!"

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR)
	rm -f *.opus *.wav

# Run tests
test: all
	@echo "Running basic pipeline test..."
	@cd examples && ./run_pipeline.sh

.PHONY: all directories opus hrtf install clean test