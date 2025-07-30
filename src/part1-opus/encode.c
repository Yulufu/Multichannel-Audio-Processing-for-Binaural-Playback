/*****************************************************************************
 * encode.c
 *
 * Open 5 WAV files (5.0 audio setup)
 * Encode blocks using Opus multistream encoder and write to a binary file
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>     /* atoi */
#include <string.h>     /* memset */
#include <sndfile.h>    /* libsndfile */
#include "common.h"
#include <opus_multistream.h>/* Opus multistream */

/* Constants */
#define NUM_CHAN 5           // Total channels (5.0 surround sound)
#define APPLICATION OPUS_APPLICATION_AUDIO
#define FRAME_SIZE 960

int main(int argc, char *argv[]) {
    char *ofile;
    FILE *fp;
    SNDFILE *sndfiles[NUM_CHAN]; // Array to hold input files for each channel
    SF_INFO sfinfo[NUM_CHAN];    // Info structs for each input file
    OpusMSEncoder *ms_encoder;   // Opus Multistream Encoder
    int err, bitrate;

    /* Ensure correct command-line arguments */
    if (argc != 8) {
        fprintf(stderr, "Usage: %s <output opus file> <input wav FL> <input wav Center> <input wav FR> <input wav RL> <input wav RR> <bitrate>\n", argv[0]);
        return 1;
    }

    ofile = argv[1];
    bitrate = atoi(argv[7]);

    /* Open the 5 input WAV files */
    sndfiles[0] = sf_open(argv[2], SFM_READ, &sfinfo[0]); // Front Left
    sndfiles[1] = sf_open(argv[3], SFM_READ, &sfinfo[1]); // Front Right
    sndfiles[2] = sf_open(argv[4], SFM_READ, &sfinfo[2]); // Center
    sndfiles[3] = sf_open(argv[5], SFM_READ, &sfinfo[3]); // Rear Left
    sndfiles[4] = sf_open(argv[6], SFM_READ, &sfinfo[4]); // Rear Right

    /* Validate each input file */
    for (int i = 0; i < NUM_CHAN; i++) {
        if (!sndfiles[i]) {
            fprintf(stderr, "Error: Input file cannot be opened: %s\n", argv[i + 2]);
            return 1;
        }
        /* Verify sampling rate and number of channels */
        if (sfinfo[i].samplerate != 48000) {
            fprintf(stderr, "Error: Input file sample rate is not 48kHz: %s\n", argv[i + 2]);
            return 1;
        }
        if (sfinfo[i].channels != 1) { // Ensure mono files
            fprintf(stderr, "Error: Input file is not mono: %s\n", argv[i + 2]);
            return 1;
        }
    }



    /* Open output binary file */
    fp = fopen(ofile, "wb");
    if (!fp) {
        fprintf(stderr, "Error: Cannot open output file: %s\n", ofile);
        return 1;
    }

    /* Setup Opus multistream encoder using surround encoder create function */
    int streams;
    int coupled_streams;
    unsigned char mapping[NUM_CHAN];
    int mapping_family = 1; // Vorbis mapping family

    ms_encoder = opus_multistream_surround_encoder_create(
        48000,                // Sample rate
        NUM_CHAN,             // Number of input channels
        mapping_family,       // Mapping family
        &streams,             // Number of streams (output)
        &coupled_streams,     // Number of coupled streams (output)
        mapping,              // Mapping array (output)
        APPLICATION,          // Application
        &err                  // Error code
    );

    if (err != OPUS_OK) {
        fprintf(stderr, "Failed to create multistream surround encoder: %s\n", opus_strerror(err));
        fclose(fp);
        return -1;
    }

    /* For debugging, print the mapping */
    printf("Streams: %d, Coupled Streams: %d\n", streams, coupled_streams);
    printf("Channel Mapping: ");
    for (int i = 0; i < NUM_CHAN; i++) {
        printf("%d ", mapping[i]);
    }
    printf("\n");

    err = opus_multistream_encoder_ctl(ms_encoder, OPUS_SET_BITRATE(bitrate));
    if (err < 0) {
        fprintf(stderr, "Failed to set bitrate: %s\n", opus_strerror(err));
        opus_multistream_encoder_destroy(ms_encoder);
        fclose(fp);
        return -1;
    }

    /* Loop Section: Read, interleave, and encode audio */
    float input_buffers[NUM_CHAN][FRAME_SIZE]; // Individual buffers for each channel
    float interleaved_buffer[FRAME_SIZE * NUM_CHAN]; // Interleaved buffer for encoding
    unsigned char tcbits[MAX_PACKET_SIZE];

    while (1) {
        sf_count_t num_frames[NUM_CHAN];
        int eof = 0;

        /* Read a block from each channel */
        for (int i = 0; i < NUM_CHAN; i++) {
            num_frames[i] = sf_readf_float(sndfiles[i], input_buffers[i], FRAME_SIZE);
            if (num_frames[i] < FRAME_SIZE) {
                eof = 1; // End of file for at least one channel
            }
        }

        /* Stop if any channel has reached EOF */
        if (eof) break;

        /* Interleave the samples */
        for (int frame = 0; frame < FRAME_SIZE; frame++) {
            for (int chan = 0; chan < NUM_CHAN; chan++) {
                interleaved_buffer[frame * NUM_CHAN + chan] = input_buffers[chan][frame];
            }
        }

        /* Encode the interleaved buffer */
        int nBytes = opus_multistream_encode_float(ms_encoder, interleaved_buffer, FRAME_SIZE, tcbits, MAX_PACKET_SIZE);
        if (nBytes < 0) {
            fprintf(stderr, "Encode failed: %s\n", opus_strerror(nBytes));
            opus_multistream_encoder_destroy(ms_encoder);
            for (int i = 0; i < NUM_CHAN; i++) sf_close(sndfiles[i]);
            fclose(fp);
            return -1;
        }

        printf("Encoded packet size: %d bytes\n", nBytes);

        /* Write packet length first */
        unsigned char packet_length_bytes[2];
        packet_length_bytes[0] = (nBytes >> 8) & 0xFF; // msb
        packet_length_bytes[1] = nBytes & 0xFF;        // lsb
        size_t write_count = fwrite(packet_length_bytes, 2, 1, fp);
        if (write_count != 1) {
            fprintf(stderr, "Error writing packet length to file\n");
            break;
        }

        /* Then write the packet data */
        write_count = fwrite(tcbits, nBytes, 1, fp);
        if (write_count != 1) {
            fprintf(stderr, "Error writing packet to file\n");
            break;
        }
    }

    /* Clean up */
    opus_multistream_encoder_destroy(ms_encoder);
    for (int i = 0; i < NUM_CHAN; i++) {
        sf_close(sndfiles[i]);
    }
    fclose(fp);

    printf("Encoding completed successfully.\n");
    return 0;
}
