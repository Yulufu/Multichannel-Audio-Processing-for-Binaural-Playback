#!/bin/bash

# Example script to run the complete audio pipeline

echo "==================================="
echo "Audio Binaural Pipeline Demo"
echo "==================================="

# Paths
ENCODE="../build/bin/encode"
DECODE="../build/bin/decode"
HRTF="../build/bin/hrtf_processor"
HRTF_DIR="../hrtf_data/MIT_KEMAR/elev0"

# Check if binaries exist
if [ ! -f "$ENCODE" ] || [ ! -f "$DECODE" ] || [ ! -f "$HRTF" ]; then
    echo "Error: Binaries not found. Please run 'make all' first."
    exit 1
fi

# Check if test audio exists
if [ ! -f "test_audio/FL.wav" ]; then
    echo "Error: Test audio files not found in test_audio/"
    echo "Please provide 5 mono WAV files (48kHz):"
    echo "  - FL.wav (Front Left)"
    echo "  - FR.wav (Front Right)"  
    echo "  - C.wav (Center)"
    echo "  - RL.wav (Rear Left)"
    echo "  - RR.wav (Rear Right)"
    exit 1
fi

# Check if HRTF data exists
if [ ! -d "$HRTF_DIR" ]; then
    echo "Error: HRTF data not found at $HRTF_DIR"
    echo "Please download and setup the MIT KEMAR database."
    exit 1
fi

# Step 1: Encode
echo ""
echo "Step 1: Encoding 5.0 surround to Opus..."
$ENCODE test_output.opus test_audio/FL.wav test_audio/C.wav test_audio/FR.wav test_audio/RL.wav test_audio/RR.wav 256000

if [ $? -ne 0 ]; then
    echo "Error: Encoding failed"
    exit 1
fi

# Step 2: Decode
echo ""
echo "Step 2: Decoding Opus to 5-channel WAV..."
$DECODE test_output.opus decoded_5ch.wav

if [ $? -ne 0 ]; then
    echo "Error: Decoding failed"
    exit 1
fi

# Step 3: HRTF Processing
echo ""
echo "Step 3: Converting to binaural audio..."
$HRTF decoded_5ch.wav binaural_output.wav $HRTF_DIR

if [ $? -eq 0 ]; then
    echo ""
    echo "==================================="
    echo "Pipeline completed successfully!"
    echo "==================================="
    echo "Output files:"
    echo "  - test_output.opus (compressed)"
    echo "  - decoded_5ch.wav (5-channel)"
    echo "  - binaural_output.wav (binaural stereo)"
    echo ""
    echo "Listen to binaural_output.wav with headphones for 3D audio!"
else
    echo "Error: HRTF processing failed"
    exit 1
fi