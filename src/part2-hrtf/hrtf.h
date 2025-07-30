#ifndef HRTF_H
#define HRTF_H

#include <sndfile.h>  /* For audio file handling */

/* Constants */
#define HRTF_SIZE 1024        // Number of samples in each HRTF filter
#define FRAME_SIZE 512        // Number of frames processed in each block
#define SUCCESS 0             // Success return code
#define ERR_FILE_OPEN -1      // Error: Unable to open a file
#define ERR_INVALID_CHANNELS -2 // Error: Input file has invalid number of channels

/* Function Prototypes */

/**
 * @brief Processes a 5-channel audio file into a binaural output using HRTFs.
 *
 * @param input_file Path to the 5-channel input WAV file.
 * @param output_file Path to the stereo binaural output WAV file.
 * @param hrtf_dir Path to the directory containing HRTF files.
 * @return int Returns SUCCESS (0) if successful, or an error code otherwise.
 */
int process_hrtf(const char *input_file, const char *output_file, const char *hrtf_dir);

#endif /* HRTF_H */
