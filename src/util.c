#include "util.h"

int read_file_source(const char* file_path, char* dest_buffer, int buffer_size) {
    FILE *fp;
    int status = UTIL_FAILURE;

    dest_buffer[0] = '\0';
    fp = fopen(file_path, "r");

    if (fp) {
        char line[FILE_LINE_BUFFER_SIZE];
        unsigned int pos = 0;
        while (fgets(line, FILE_LINE_BUFFER_SIZE, fp) != NULL) {
            unsigned int line_len = strnlen(line, FILE_LINE_BUFFER_SIZE);
            if (line_len + pos >= buffer_size) {
                // Prevent buffer overflow on strcpy
#ifdef UTIL_DEBUG
                printf("read_file_source '%s' exceeds buffer size %d\n", file_path, buffer_size);
#endif
                break;
            }
            strcpy(dest_buffer + pos, line);
            pos += line_len;
        }
        dest_buffer[pos] = '\0';  // set last char to null terminating char
        status = UTIL_SUCCESS;
        fclose(fp);
    }

    return status;
}

struct bounding_box bounding_box(float *positions_xyz, int len) {
    struct bounding_box bounds = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};

    for (int i = 0; i < len / 3; i++) {
        float x = positions_xyz[i * 3];
        float y = positions_xyz[i * 3 + 1];
        float z = positions_xyz[i * 3 + 2];

        if (x < bounds.x_min) {
            bounds.x_min = x;
        }
        if (x > bounds.x_max) {
            bounds.x_max = x;
        }
        if (y < bounds.y_min) {
            bounds.y_min = y;
        }
        if (y > bounds.y_max) {
            bounds.y_max = y;
        }
        if (z < bounds.z_min) {
            bounds.z_min = z;
        }
        if (z > bounds.z_max) {
            bounds.z_max = z;
        }
    }

    return bounds;
}

void invert_float_buffer(float* buffer, int len) {
    for (int i = 0; i < len; i++) {
        buffer[i] *= -1.0f;
    }
}
