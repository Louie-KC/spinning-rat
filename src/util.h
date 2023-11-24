#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

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
struct bounding_box model_bounding_box(float *vertex_buffer, int n_vertices);

// Shift all vertices of a model/mesh to be centered at [0, 0, 0].
// Assumes `bounds` is the result of model_bounding_box with the same model.
void centre_model(float *vertex_buffer, int n_vertices, struct bounding_box bounds);

// Scale a model/mesh to be at most 2 unit lengths ([-1, 1] if centred).
// NOTE: This removes any scale/length of unit lengths (metres, feet, etc) from mesh.
// Assumes `bounds` is the result of model_bounding_box with the same model.
void normalise_model_scale(float* vertex_buffer, int n_vertices, struct bounding_box bounds);

void invert_float_buffer(float* buffer, int len);

// Reads in a 3D model from `file_path` to the provided buffers.
// Allocates memory for `vertex_buffer`, `index_buffer` and `normals_buffer`.
// Stores the length of the `vertex_buffer` at `n_vertices`.
// Stores the length of the `index_buffer` at `n_indices`.
// Note, the length of the `normals_buffer` is the same as the `vertex_buffer`.
// Set `post_process_flags` with `UTIL_PROCESS...` to perform some processing on model.
// 0 if none is needed.
int load_model(const char* file_path,
               float** vertex_buffer,
               unsigned int** index_buffer,
               float** normals_buffer,
               unsigned int *n_vertices,
               unsigned int *n_indices,
               unsigned int post_process_flags,
               int flip_winding_order);

#endif