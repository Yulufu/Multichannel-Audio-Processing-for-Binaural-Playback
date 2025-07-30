#!/bin/bash

echo "==================================="
echo "Audio Binaural Pipeline Setup"
echo "==================================="

# Detect OS
OS=$(uname -s)

# Function to check if a command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Install dependencies based on OS
install_dependencies() {
    echo "Installing dependencies..."
    
    if [[ "$OS" == "Darwin" ]]; then
        # macOS
        if ! command_exists brew; then
            echo "Error: Homebrew not found. Please install Homebrew first."
            echo "Visit: https://brew.sh"
            exit 1
        fi
        
        echo "Installing Opus and libsndfile via Homebrew..."
        brew install opus libsndfile
        
    elif [[ "$OS" == "Linux" ]]; then
        # Linux
        if command_exists apt-get; then
            # Debian/Ubuntu
            echo "Installing Opus and libsndfile via apt..."
            sudo apt-get update
            sudo apt-get install -y libopus-dev libsndfile1-dev build-essential
        elif command_exists yum; then
            # RHEL/CentOS/Fedora
            echo "Installing Opus and libsndfile via yum..."
            sudo yum install -y opus-devel libsndfile-devel gcc make
        else
            echo "Error: Unsupported Linux distribution"
            exit 1
        fi
    else
        echo "Error: Unsupported operating system"
        exit 1
    fi
}

# Create directory structure
create_directories() {
    echo "Creating project directories..."
    mkdir -p src/part1-opus src/part2-hrtf
    mkdir -p build/bin
    mkdir -p examples/test_audio
    mkdir -p docs
    mkdir -p hrtf_data/MIT_KEMAR
}

# Check for required tools
check_tools() {
    echo "Checking for required tools..."
    
    if ! command_exists gcc; then
        echo "Error: gcc not found. Please install a C compiler."
        exit 1
    fi
    
    if ! command_exists sox; then
        echo "Warning: SoX not found. You'll need it to resample HRTF files."
        echo "Install with: brew install sox (macOS) or apt-get install sox (Linux)"
    fi
}

# Download MIT KEMAR HRTF database info
setup_hrtf() {
    echo ""
    echo "==================================="
    echo "HRTF Database Setup"
    echo "==================================="
    echo "Please download the MIT KEMAR HRTF database manually:"
    echo "1. Visit: http://sound.media.mit.edu/resources/KEMAR.html"
    echo "2. Download the 'full' dataset"
    echo "3. Extract the elev0 folder to: ./hrtf_data/MIT_KEMAR/"
    echo ""
    echo "After downloading, use SoX to resample to 48kHz:"
    echo "cd hrtf_data/MIT_KEMAR/elev0"
    echo "for f in *.wav; do sox \"\$f\" -r 48000 \"48k_\$f\"; done"
    echo ""
}

# Main setup
main() {
    echo "Starting setup..."
    
    # Check OS compatibility
    if [[ "$OS" != "Darwin" && "$OS" != "Linux" ]]; then
        echo "Error: This script only supports macOS and Linux"
        exit 1
    fi
    
    # Run setup steps
    install_dependencies
    create_directories
    check_tools
    
    echo ""
    echo "Building the project..."
    make clean
    make all
    
    if [ $? -eq 0 ]; then
        echo ""
        echo "==================================="
        echo "Setup completed successfully!"
        echo "==================================="
        echo "Binaries are in: build/bin/"
        echo ""
        echo "Next steps:"
        echo "1. Download HRTF database (see instructions above)"
        echo "2. Prepare your 5 mono WAV files (48kHz)"
        echo "3. Run the pipeline:"
        echo "   ./build/bin/encode output.opus FL.wav C.wav FR.wav RL.wav RR.wav 256000"
        echo "   ./build/bin/decode output.opus decoded_5ch.wav"
        echo "   ./build/bin/hrtf_processor decoded_5ch.wav binaural.wav ./hrtf_data/MIT_KEMAR/elev0/"
    else
        echo "Error: Build failed. Please check the error messages above."
        exit 1
    fi
    
    setup_hrtf
}

# Run main function
main