#ifndef SCN_OBJ_H
#define SCN_OBJ_H

#include <stdlib.h>
#include <glad/glad.h>

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "matrix.h"
#include "util.h"

// #define SCN_OBJ_DEBUG

#define SCN_OBJ_IMPORT_CENTRED              0b10000000
#define SCN_OBJ_IMPORT_UNIT_RESCALE         0b01000000
#define SCN_OBJ_IMPORT_FLIP_NORMALS         0b00100000
#define SCN_OBJ_IMPORT_FLIP_WINDING_ORDER   0b00010000

typedef struct scene_object {
    unsigned int VBO;
    unsigned int VAO;
    unsigned int EBO;
    float* vertex_buffer;
    float* normals_buffer;
    unsigned int* index_buffer;
    unsigned int vertex_buffer_len;
    unsigned int index_buffer_len;
    mat4 model_matrix;
    struct scene_object* parent;
} scene_object;

struct scene_object scn_obj_load(const char* file_path, unsigned int flags);

// Shift all vertices of a model/mesh to be centered at [0, 0, 0].
// Assumes `bounds` is the result of model_bounding_box with the same model.
void scn_obj_centre_model(float *vertex_buffer, int n_vertices, struct bounding_box bounds);

// Scale a model/mesh to be at most 2 unit lengths ([-1, 1] if centred).
// NOTE: This removes any scale/length of unit lengths (metres, feet, etc) from mesh.
// Assumes `bounds` is the result of model_bounding_box with the same model.
void normalise_model_scale(float* vertex_buffer, int n_vertices, struct bounding_box bounds);

// Creates, binds, and feeds data to a scene objects buffer objects.
// Invokes deletion methods on the scene objects buffers objects prior
// to generating and binding new buffers.
void scn_obj_set_buffers(scene_object* scn_obj);

// Calculate the mvp matrix for a scene object
mat4 scn_obj_mvp(scene_object* scn_obj, mat4 view, mat4 proj);

void scn_obj_clean(scene_object* scn_obj);

#endif