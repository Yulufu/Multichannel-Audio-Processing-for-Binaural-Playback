#ifndef COMMON_H
#define COMMON_H

/* Constants for Opus and Audio Processing */
#define NUM_AMBISONIC_CHANNELS 4   // Ambisonics (1st order)
#define NUM_BINAURAL_CHANNELS 2    // Binaural audio
#define NUM_5_0_CHANNELS 5         // 5.0 Surround Sound
#define APPLICATION OPUS_APPLICATION_AUDIO
#define MAX_PACKET_SIZE 32768      // Max size of an Opus packet (bytes)
#define HRTF_SAMPLE_RATE 48000     // HRTF assumes 48kHz

/* Network Configuration */
#define PORT 12345                 // Port for streaming, subject to change
#define MAX_CLIENTS 5              // Maximum number of clients for streaming

/* Error Codes */
#define SUCCESS 0
#define ERR_FILE_OPEN -1
#define ERR_ENCODING -2
#define ERR_DECODING -3
#define ERR_NETWORK -4

/* Debugging Macros */
#ifdef DEBUG
    #define LOG(msg, ...) fprintf(stderr, "LOG: " msg "\n", ##__VA_ARGS__)
#else
    #define LOG(msg, ...) // No logging
#endif

#endif /* COMMON_H */
