#ifndef SCN_OBJ_H
#define SCN_OBJ_H

#include <stdlib.h>
#include <glad/glad.h>

#include "matrix.h"
#include "util.h"

typedef struct scene_object {
    unsigned int VBO;
    unsigned int VAO;
    unsigned int EBO;
    float* vertex_buffer;
    unsigned int* index_buffer;
    unsigned int vertex_buffer_len;
    unsigned int index_buffer_len;
    mat4 model_matrix;
    struct scene_object* parent;
} scene_object;

void scn_obj_init(scene_object* scn_obj);

void scn_obj_clean(scene_object* scn_obj);

#endif