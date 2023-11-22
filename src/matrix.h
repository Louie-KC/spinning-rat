#ifndef MATRIX_H
#define MATRIX_H

#include <stdio.h>
#include <math.h>
#define _USE_MATH_DEFINES

typedef struct matrix3f {
    float data[9];
} mat3;

typedef struct matrix4f {
    float data[16];
} mat4;

float degrees_to_radians(float degrees);

void set_identity_n(float data[], int n);
void set_zero_n(float data[], int n);
void multiply_matrix_n(float *dest, float *with, int n);
void print_matrix_n(float data[], int n);

mat3 mat3_identity();
mat3 mat3_zero();
void mat3_set_identity(mat3 *mat);
void mat3_set_zero(mat3 *mat);
void mat3_print(mat3 *mat);
void mat3_multiply(mat3 *dest, mat3 *with);
void mat3_translate(mat3 *mat, float x, float y);
void mat3_rotate(mat3 *mat, float radians);
void mat3_scale(mat3 *mat, float x, float y);
void mat3_transform(mat3 *mat, float x, float y, float radians, float sx, float sy);

mat4 mat4_identity();
mat4 mat4_zero();
void mat4_set_identity(mat4 *mat);
void mat4_set_zero(mat4 *mat);
void mat4_print(mat4 *mat);
void mat4_multiply(mat4 *dest, mat4 *with);
void mat4_translate(mat4 *mat, float x, float y, float z);
void mat4_rotate_x(mat4 *mat, float radians);
void mat4_rotate_y(mat4 *mat, float radians);
void mat4_rotate_z(mat4 *mat, float radians);
void mat4_scale(mat4 *mat, float x, float y, float z);

#endif