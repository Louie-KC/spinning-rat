#ifndef SCN_OBJ_H
#define SCN_OBJ_H

#include <stdlib.h>
#include <glad/glad.h>

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "matrix.h"
#include "vector.h"
#include "util.h"

#define SCN_OBJ_DEBUG

#define SCN_OBJ_IMPORT_CENTRED              0b10000000
#define SCN_OBJ_IMPORT_UNIT_RESCALE         0b01000000
#define SCN_OBJ_IMPORT_FLIP_NORMALS         0b00100000
#define SCN_OBJ_IMPORT_FLIP_WINDING_ORDER   0b00010000

typedef struct face {
    unsigned int indices[3];
} face;

typedef struct scene_object {
    unsigned int VBO;
    unsigned int VAO;
    unsigned int EBO;
    unsigned int texture;
    unsigned int shader_prog;
    vec3 *vertex_buffer;
    vec3 *normal_buffer;
    vec2 *tex_coord_buffer;
    face *index_buffer;
    unsigned int vertex_buffer_len;
    unsigned int index_buffer_len;
    mat4 model_matrix;
    struct scene_object* parent;
} scene_object;

struct scene_object scn_obj_load(const char* file_path, unsigned int flags);

// Shift all vertices of a model/mesh to be centered at [0, 0, 0].
// Assumes `bounds` is the result of model_bounding_box with the same model.
void scn_obj_centre_model(vec3 *vertex_buffer, int n_vertices, struct bounding_box bounds);

// Scale a model/mesh to be at most 2 unit lengths ([-1, 1] if centred).
// NOTE: This removes any scale/length of unit lengths (metres, feet, etc) from mesh.
// Assumes `bounds` is the result of model_bounding_box with the same model.
void normalise_model_scale(vec3* vertex_buffer, int n_vertices, struct bounding_box bounds);

// Creates, binds, and feeds data to a scene objects buffer objects.
// Invokes deletion methods on the scene objects buffers objects prior
// to generating and binding new buffers.
void scn_obj_set_buffers(scene_object* scn_obj);

// Creates a texture buffer for the scene object, sets tex parameters, then loads the
// texture at the specified path, binding it to the scene object.
void scn_obj_load_texture(scene_object* scn_obj, char* texture_path);

// Calculate the mvp matrix for a scene object
mat4 scn_obj_mvp(scene_object* scn_obj, mat4 view, mat4 proj);

void scn_obj_clean(scene_object* scn_obj);

#endif