#include <stdio.h>
#include <stdlib.h>     /* atoi */
#include <string.h>     /* memset */
#include <sndfile.h>    /* libsndfile */
#include "common.h"
#include <opus_multistream.h> /* Opus multistream */
#include <stdbool.h>

#define NUM_CHAN 5           // Total channels (5.0 surround sound)
#define BINAURAL_CHAN 2      // Output is stereo (binaural)
#define SAMP_RATE 48000
#define FRAMES_PER_BUFFER 960

int main(int argc, char *argv[]) {
    char *ifile, *ofile;
    FILE *fp;
    SNDFILE *sndfile;
    SF_INFO sfinfo;
    OpusMSDecoder *ms_decoder;
    int err;

    /* Parse command-line arguments */
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input opus file> <output wav file>\n", argv[0]);
        return 1;
    }

    ifile = argv[1];
    ofile = argv[2];

    printf("Input Opus file: %s\n", ifile);
    printf("Output WAV file: %s\n", ofile);

    /* Open binary input file */
    fp = fopen(ifile, "rb");
    if (!fp) {
        fprintf(stderr, "Error: Cannot open input file %s\n", ifile);
        return 1;
    }

    /* Set WAV file header */
    sfinfo.samplerate = SAMP_RATE;
    sfinfo.channels = NUM_CHAN; // Multichannel output
    sfinfo.format = SF_FORMAT_WAV | SF_FORMAT_FLOAT;

    /* Open WAV output file */
    sndfile = sf_open(ofile, SFM_WRITE, &sfinfo);
    if (!sndfile) {
        fprintf(stderr, "Error: Cannot open output file %s\n", ofile);
        fclose(fp);
        return 1;
    }

    /* Setup Opus multistream decoder using mapping family 1 */
    int streams;
    int coupled_streams;
    unsigned char mapping[NUM_CHAN];

    /* For mapping family 1 with 5 channels */
    streams = 3;
    coupled_streams = 2;
    mapping[0] = 0; // Front Left
    mapping[1] = 1; // Front Right
    mapping[2] = 2; // Center
    mapping[3] = 3; // Rear Left
    mapping[4] = 4; // Rear Right

    /* Create the decoder */
    ms_decoder = opus_multistream_decoder_create(
        SAMP_RATE,            // Sample rate
        NUM_CHAN,             // Total channels
        streams,              // Number of streams
        coupled_streams,      // Coupled streams
        mapping,              // Channel mapping
        &err                  // Error variable
    );

    if (err < 0) {
        fprintf(stderr, "Failed to create multi-stream decoder: %s\n", opus_strerror(err));
        fclose(fp);
        sf_close(sndfile);
        return -1;
    }

    /* Decoding loop */
    while (1) {
        int count;
        unsigned char rcbits[MAX_PACKET_SIZE];
        float decoded_audio[NUM_CHAN * FRAMES_PER_BUFFER]; // Buffer for decoded 5.0 audio
        int nBytes;

        /* Read packet length */
        unsigned char packet_length_bytes[2];
        if ((count = fread(packet_length_bytes, 2, 1, fp)) != 1) {
            if (feof(fp)) {
                printf("End of file reached\n");
                break; // End of file
            }
            fprintf(stderr, "Error reading packet length (count = %d)\n", count);
            break;
        }
        /* Form packet length */
        nBytes = (packet_length_bytes[0] << 8) | packet_length_bytes[1];

        /* Read encoded packet */
        if ((count = fread(rcbits, nBytes, 1, fp)) != 1) {
            fprintf(stderr, "Error reading encoded packet (count = %d, nBytes = %d)\n", count, nBytes);
            break;
        }

        /* Decode packet */
        int frame_size = opus_multistream_decode_float(
            ms_decoder,
            rcbits,         // Input packet
            nBytes,         // Packet size
            decoded_audio,  // Decoded 5.0 output
            FRAMES_PER_BUFFER, // Frame size
            0               // No FEC
        );

        if (frame_size < 0) {
            fprintf(stderr, "Decode failed: %s\n", opus_strerror(frame_size));
            break;
        }

        if (frame_size != FRAMES_PER_BUFFER) {
            fprintf(stderr, "Warning: Frame size mismatch! Expected %d, got %d\n", FRAMES_PER_BUFFER, frame_size);
        }

        /* Write decoded audio directly to WAV file */
        sf_write_float(sndfile, decoded_audio, frame_size * NUM_CHAN);
    }

    /* Clean up */
    opus_multistream_decoder_destroy(ms_decoder);
    sf_close(sndfile);
    fclose(fp);

    printf("Decoding completed successfully.\n");
    return 0;
}
