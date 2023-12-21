#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include "util.h"
#include "matrix.h"
#include "vector.h"

#define SHADER_SOURCE_MAX_LEN 4096
#define SHADER_INFO_BUFFER_SIZE 512

typedef unsigned int shader_program;

shader_program shader_create_program(const char* vertex_path, const char* frag_path);

int shader_compile(unsigned int* shader, int type, const char* source);

shader_program shader_link(shader_program program, unsigned int vertex, unsigned int fragment);

void shader_set_uniform_float(shader_program program, const char* name, float value);

void shader_set_uniform_vec2(shader_program program, const char* name, vec2 vec);

void shader_set_uniform_vec3(shader_program program, const char* name, vec3 vec);

void shader_set_uniform_vec4(shader_program program, const char* name, vec4 vec);

void shader_set_uniform_mat3(shader_program program, const char* name, mat3 mat);

void shader_set_uniform_mat4(shader_program program, const char* name, mat4 mat);

void shader_set_uniform_tex_i(shader_program program, const char* name, unsigned int tex_id);

#endif