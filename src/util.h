#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// #define UTIL_DEBUG

#define FILE_LINE_BUFFER_SIZE 256
#define FILE_BUFFER_SIZE 65536 * 128

#define UTIL_SUCCESS 1
#define UTIL_FAILURE 0

#define UTIL_PROCESS_CENTRE_MODEL 0b10000000
#define UTIL_PROCESS_SCALE_MODEL  0b01000000
#define UTIL_PROCESS_FLIP_NORMALS 0b00100000

struct bounding_box {
    float x_min;
    float x_max;
    float y_min;
    float y_max;
    float z_min;
    float z_max;
};

// Read textual file contents.
// Max line length: FILE_LINE_BUFFER_SIZE
// Max buffer size: FILE_BUFFER_SIZE
int read_file_source(const char* file_path, char* dest_buffer, int buffer_size);

// Calculate the bounding box for a set of vertices.
struct bounding_box bounding_box(float *vertex_buffer, int n_vertices);

void invert_float_buffer(float* buffer, int len);

#endif