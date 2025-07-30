#include "hrtf.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <input_file> <output_file> <hrtf_dir>\n", argv[0]);
        return 1;
    }

    const char *input_file = argv[1];
    const char *output_file = argv[2];
    const char *hrtf_dir = argv[3];

    int result = process_hrtf(input_file, output_file, hrtf_dir);
    if (result != SUCCESS) {
        fprintf(stderr, "Error: HRTF processing failed with code %d\n", result);
        return result;
    }

    printf("HRTF processing completed successfully.\n");
    return 0;
}
