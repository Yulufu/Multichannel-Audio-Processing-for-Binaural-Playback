# Multichannel Audio Processing for Binaural Playback

A comprehensive C-based audio processing pipeline that encodes/decodes 5.0 surround sound using the Opus codec and converts it to binaural audio for immersive headphone playback using HRTFs.

## Overview
This project implements a complete offline audio processing pipeline with two main components:

### Part 1: Opus Surround Sound Codec
- Encode 5 separate mono WAV files (representing 5.0 surround channels) into a single compressed Opus multistream file
- Decode the Opus multistream file back to a 5-channel WAV file
- Efficient 3-stream format: 2 coupled streams for front/rear pairs, 1 uncoupled for center

### Part 2: HRTF Binaural Processing
- Convert 5.0 surround sound to binaural stereo for headphone playback
- Uses MIT KEMAR HRTF database for realistic spatial audio
- Block-based convolution processing for efficiency

### Channel Configuration (5.0 Surround)
- Front Left (FL) - 30° azimuth
- Front Right (FR) - 330° azimuth
- Center (C) - 0° azimuth
- Rear Left (RL) - 120° azimuth
- Rear Right (RR) - 240° azimuth

## Prerequisites
### macOS (Homebrew)
```bash
brew install opus libsndfile
```
### Ubuntu/Debian
```bash
sudo apt-get install libopus-dev libsndfile1-dev
```
### Other Systems
Install the following libraries:

- Opus - Audio codec library
- libsndfile - Audio file I/O library

## HRTF Database
The project uses the MIT KEMAR HRTF database. You'll need to:

1. Download the MIT KEMAR dataset (full_elev0 directory)
2. Convert HRTF files to 48kHz using SoX:
```bash
sox input.wav -r 48000 output.wav
```

## Quick Start
```bash
# 1. Run setup script
chmod +x setup.sh
./setup.sh

# 2. Build all components
make all

# 3. Run the complete pipeline
./encode output.opus FL.wav C.wav FR.wav RL.wav RR.wav 256000
./decode output.opus decoded_5ch.wav
./hrtf_processor decoded_5ch.wav binaural.wav ./hrtf_data/MIT_KEMAR/elev0/
```
## Build
### Using Make (Recommended)
```bash
make all          # Build all components
make clean        # Clean build artifacts
make install      # Install to /usr/local/bin (requires sudo)
```

### Using Build Scripts
```bash
# Part 1: Opus Encoder/Decoder
chmod +x build.sh
./build.sh

# Part 2: HRTF Processor
chmod +x build1.sh
./build1.sh
```
The build system automatically detects your architecture (Intel/Apple Silicon Mac/Linux) and sets the appropriate paths.

## Usage
### Complete Pipeline Example
1. Encode 5 mono WAV files to Opus:
```bash
./encode output.opus front_left.wav center.wav front_right.wav rear_left.wav rear_right.wav 256000
```

2. Decode Opus to 5-channel WAV:
```bash
./decode output.opus decoded_5channel.wav
```

3. Process to binaural audio:
```bash
./hrtf_processor decoded_5channel.wav binaural_output.wav /path/to/hrtf/directory/
```

## Individual Components
### Opus Encoding
```bash
./encode <output.opus> <FL.wav> <C.wav> <FR.wav> <RL.wav> <RR.wav> <bitrate>
```
- Input: 5 mono WAV files (48 kHz)
- Output: Compressed Opus file
- Bitrate: e.g., 256000 for 256 kbps

### Opus Decoding
```bash
./decode <input.opus> <output.wav>
```
- Input: Opus multistream file
- Output: 5-channel WAV file

### HRTF Binaural Processing
```bash
./hrtf_processor <input_5ch.wav> <output_binaural.wav> <hrtf_directory>
```
- Input: 5-channel WAV file
- Output: Stereo binaural WAV file
- HRTF directory: Path to MIT KEMAR HRTF files

## Technical Details
### Part 1: Opus Codec Specifications
- Sample Rate: 48 kHz (required)
- Frame Size: 960 samples
- Opus Application: OPUS_APPLICATION_AUDIO
- Mapping Family: 1 (Vorbis channel order)
- Stream Configuration: 3 streams (2 coupled, 1 uncoupled)
- Packet Format: 2-byte length header + Opus data

### Part 2: HRTF Processing
- HRTF Database: MIT KEMAR full_elev0
- Processing Method: Block-based FFT convolution
- Block Size: 512 samples
- History Buffer: Maintains convolution overlap
- Output Format: 32-bit floating-point PCM

### Architecture Support
The build scripts support:

- Intel Macs (x86_64)
- Apple Silicon Macs (arm64)
- Linux and other systems

## Project Structure
```bash
audio-binaural-pipeline/
├── README.md
├── LICENSE
├── Makefile
├── setup.sh
├── .gitignore
├── src/
│   ├── part1-opus/
│   │   ├── encode.c
│   │   ├── decode.c
│   │   └── common.h
│   └── part2-hrtf/
│       ├── main.c
│       ├── hrtf.c
│       ├── hrtf.h
│       └── common.h
├── build/
│   ├── build_opus.sh
│   └── build_hrtf.sh
├── examples/
│   ├── test_audio/
│   └── run_pipeline.sh
├── docs/
│   ├── presentation.pdf
│   └── report.pdf
└── hrtf_data/
    └── README.md 

```

## Applications
This pipeline is ideal for:

- Virtual Reality (VR): Creating immersive spatial audio for VR headsets
- Gaming: 3D positional audio for competitive and immersive gaming
- Film & Music: Converting surround mixes for headphone listening
- Streaming: Efficient compression of multichannel audio for bandwidth-limited applications
- Accessibility: Making surround content accessible via standard headphones

## Known Limitations
- Input files must be exactly 48 kHz sample rate
- Encoder expects mono WAV files as input
- No built-in resampling support
- Simple packet format (not Ogg container)
- Fixed HRTF angles (not interpolated)
- Offline processing only (not real-time)