#include "hrtf.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#define HRTF_SIZE 1024
#define FRAME_SIZE 512

int process_hrtf(const char *input_file, const char *output_file, const char *hrtf_dir) {
    SNDFILE *infile, *outfile;
    SF_INFO sfinfo_in = {0}, sfinfo_out = {0};

    /* Open input multichannel file */
    infile = sf_open(input_file, SFM_READ, &sfinfo_in);
    if (!infile) {
        fprintf(stderr, "Error: Cannot open input file %s\n", input_file);
        return ERR_FILE_OPEN;
    }

    if (sfinfo_in.channels != 5) {
        fprintf(stderr, "Error: Input file must have 5 channels (5.0 surround).\n");
        sf_close(infile);
        return ERR_INVALID_CHANNELS;
    }

    /* Configure output stereo file */
    sfinfo_out = sfinfo_in;
    sfinfo_out.channels = 2; // Binaural stereo
    outfile = sf_open(output_file, SFM_WRITE, &sfinfo_out);
    if (!outfile) {
        fprintf(stderr, "Error: Cannot open output file %s\n", output_file);
        sf_close(infile);
        return ERR_FILE_OPEN;
    }

    /* Load HRTFs */
    /* Load HRTFs */
    float hrtf_left[5][HRTF_SIZE] = {{0}};
    float hrtf_right[5][HRTF_SIZE] = {{0}};
    SF_INFO hrtf_info = {0};
    char hrtf_path[512];

    const char *hrtf_left_files[5] = {
        "L0e330a.wav", // Front Left
        "L0e030a.wav", // Front Right
        "L0e000a.wav", // Center
        "L0e240a.wav", // Rear Left
        "L0e120a.wav"  // Rear Right
    };

    const char *hrtf_right_files[5] = {
        "R0e330a.wav", // Front Left
        "R0e030a.wav", // Front Right
        "R0e000a.wav", // Center
        "R0e240a.wav", // Rear Left
        "R0e120a.wav"  // Rear Right
    };

    for (int ch = 0; ch < 5; ++ch) {
        /* Load Left HRTF */
        snprintf(hrtf_path, sizeof(hrtf_path), "%s/%s", hrtf_dir, hrtf_left_files[ch]);
        SNDFILE *hrtf_file = sf_open(hrtf_path, SFM_READ, &hrtf_info);
        if (!hrtf_file) {
            fprintf(stderr, "Error: Cannot open HRTF file %s\n", hrtf_path);
            return ERR_FILE_OPEN;
        }
        sf_read_float(hrtf_file, hrtf_left[ch], HRTF_SIZE);
        sf_close(hrtf_file);

        /* Load Right HRTF */
        snprintf(hrtf_path, sizeof(hrtf_path), "%s/%s", hrtf_dir, hrtf_right_files[ch]);
        hrtf_file = sf_open(hrtf_path, SFM_READ, &hrtf_info);
        if (!hrtf_file) {
            fprintf(stderr, "Error: Cannot open HRTF file %s\n", hrtf_path);
            return ERR_FILE_OPEN;
        }
        sf_read_float(hrtf_file, hrtf_right[ch], HRTF_SIZE);
        sf_close(hrtf_file);
    }

    // /* Normalize HRTF Filters */
    // for (int ch = 0; ch < 5; ++ch) {
    //     float left_sum = 0.0f, right_sum = 0.0f;
    //     for (int k = 0; k < HRTF_SIZE; ++k) {
    //         left_sum += hrtf_left[ch][k] * hrtf_left[ch][k];
    //         right_sum += hrtf_right[ch][k] * hrtf_right[ch][k];
    //     }
    //     left_sum = sqrt(left_sum);
    //     right_sum = sqrt(right_sum);
    //     for (int k = 0; k < HRTF_SIZE; ++k) {
    //         hrtf_left[ch][k] /= left_sum;
    //         hrtf_right[ch][k] /= right_sum;
    //     }
    // }


    /* Allocate buffers */
    float input_buffer[FRAME_SIZE * 5] = {0};  // 5 channels
    float output_buffer[FRAME_SIZE * 2] = {0}; // Binaural stereo

    /* Initialize input history buffers */
    float input_history[5][HRTF_SIZE] = {{0}};

    /* Process each frame */
    sf_count_t read_frames;
    while ((read_frames = sf_readf_float(infile, input_buffer, FRAME_SIZE)) > 0) {
        for (int i = 0; i < read_frames; ++i) {
            float left = 0.0f;
            float right = 0.0f;

            /* Update input history for each channel */
            for (int ch = 0; ch < 5; ++ch) {
                /* Shift history */
                memmove(&input_history[ch][1], &input_history[ch][0], (HRTF_SIZE - 1) * sizeof(float));
                /* Add new sample */
                input_history[ch][0] = input_buffer[i * 5 + ch];
            }

            /* Convolve input with HRTFs */
            for (int ch = 0; ch < 5; ++ch) {
                for (int k = 0; k < HRTF_SIZE; ++k) {
                    left += input_history[ch][k] * hrtf_left[ch][k];
                    right += input_history[ch][k] * hrtf_right[ch][k];
                }
            }

            /* Store output */
            output_buffer[i * 2] = left;
            output_buffer[i * 2 + 1] = right;
        }

        /* Write processed audio to output file */
        sf_writef_float(outfile, output_buffer, read_frames);
    }

    /* Clean up */
    sf_close(infile);
    sf_close(outfile);

    return SUCCESS;
}
