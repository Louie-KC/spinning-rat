#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#define FILE_LINE_BUFFER_SIZE 256
#define FILE_BUFFER_SIZE 65536 * 128

#define UTIL_SUCCESS 1
#define UTIL_FAILURE 0

#define UTIL_PROCESS_CENTRE_MODEL 0b10000000
#define UTIL_PROCESS_SCALE_MODEL  0b01000000

struct bounding_box {
    float x_min;
    float x_max;
    float y_min;
    float y_max;
    float z_min;
    float z_max;
};


int read_file_source(const char* file_path, char* dest_buffer);
struct bounding_box model_bounding_box(float *vertex_buffer, int n_vertices);
void centre_model(float *vertex_buffer, int n_vertices, struct bounding_box bounds);
void normalise_model_scale(float* vertex_buffer, int n_vertices, struct bounding_box bounds);
int load_model(const char* file_path,
               float** vertex_buffer,
               unsigned int** index_buffer,
               int *n_vertices,
               int *n_indices,
               unsigned int flags);


#endif