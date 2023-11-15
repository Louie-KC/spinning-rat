#include <stdio.h>
#include <math.h>
#include "matrix.h"

#define _USE_MATH_DEFINES

float degrees_to_radians(float degrees) {
    return degrees * (M_PI/180);
}

void set_identity_n(float data[], int n) {
    unsigned short i = 0;
    unsigned short row = 0;
    unsigned short max_idx = n * n;

    while (i < max_idx) {
        if ((i % n) == row) {
            data[i] = 1;
        } else {
            data[i] = 0;
        }
        i++;
        row = i / n;
    }
}

void set_zero_n(float data[], int n) {
    unsigned int max_idx = n * n;
    for (unsigned int i = 0; i < max_idx; ++i) {
        data[i] = 0;
    }
}

void multiply_matrix_n(float *dest, float *with, int n) {
    float result[n * n];

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            unsigned int idx = n * i + j;
            result[idx] = 0;
            for (int k = 0; k < n; k++) {
                result[idx] += dest[n * i + k] * with[n * k + j];
            }
        }
    }

    // copy result to dest
    unsigned int i = 0;
    unsigned int max_idx = n * n;
    while (i < max_idx) {
        dest[i] = result[i];
        i++;
    }
}

void print_matrix_n(float data[], int n) {
    unsigned short i = 0;
    unsigned short max_idx = n * n;
    while (i < max_idx) {
        printf("%f ", data[i]);
        i++;
        if (i % n == 0) {
            printf("\n");
        }
    }
}

// 3x3 Matrix

mat3 mat3_identity() {
    mat3 result;
    mat3_set_identity(&result);
    return result;
}

mat3 mat3_zero() {
    mat3 result;
    mat3_set_zero(&result);
    return result;
}

void mat3_set_identity(mat3 *mat) {
    set_identity_n(mat->data, 3);
}

void mat3_set_zero(mat3 *mat) {
    set_zero_n(mat->data, 3);
}

void mat3_print(mat3 *mat) {
    print_matrix_n(mat->data, 3);
}

void mat3_multiply(mat3 *dest, mat3 *with) {
    multiply_matrix_n(dest->data, with->data, 3);
}

void mat3_translate(mat3 *mat, float x, float y) {
    mat3 t = {{1.0f, 0.0f, x, 0.0f, 1.0f, y, 0.0f, 0.0f, 1.0f}};
    mat3_multiply(mat, &t);
}

void mat3_rotate(mat3 *mat, float radians) {
    float s = (float) sin((double) radians);
    float c = (float) cos((double) radians);

    mat3 r = {{c, s, 0.0f, -s, c, 0.0f, 0.0f, 0.0f, 1.0f}};

    mat3_multiply(mat, &r);
}

void mat3_scale(mat3 *mat, float x, float y) {
    mat3 s = {{x, 0.0f, 0.0f, 0.0f, y, 0.0f, 0.0f, 0.0f, 1.0f}};    
    mat3_multiply(mat, &s);
}

void mat3_transform(mat3 *mat, float x, float y, float radians, float sx, float sy) {
    mat3_translate(mat, x, y);
    mat3_rotate(mat, radians);
    mat3_scale(mat, sx, sy);
}

// 4x4 Matrix

mat4 mat4_identity() {
    mat4 result;
    mat4_set_identity(&result);
    return result;
}

mat4 mat4_zero() {
    mat4 result;
    mat4_set_zero(&result);
    return result;
}

void mat4_set_identity(mat4 *mat) {
    set_identity_n(mat->data, 4);
}

void mat4_set_zero(mat4 *mat) {
    set_zero_n(mat->data, 4);
}

void mat4_print(mat4 *mat) {
    print_matrix_n(mat->data, 4);
}

void mat4_multiply(mat4 *dest, mat4 *with) {
    multiply_matrix_n(dest->data, with->data, 4);
}

void mat4_translate(mat4 *mat, float x, float y, float z) {
    mat4 t = {{1.0f, 0.0f, 0.0f, x,
               0.0f, 1.0f, 0.0f, y,
               0.0f, 0.0f, 1.0f, z,
               0.0f, 0.0f, 0.0f, 1.0f}};

    mat4_multiply(mat, &t);
}

void mat4_rotate_x(mat4 *mat, float radians) {
    float s = (float) sin((double) radians);
    float c = (float) cos((double) radians);

    mat4 r = {{1.0f, 0.0f, 0.0f, 0.0f,
               0.0f,    c,   -s, 0.0f,
               0.0f,    s,    c, 0.0f,
               0.0f, 0.0f, 0.0f, 1.0f}};

    mat4_multiply(mat, &r);
}

void mat4_rotate_y(mat4 *mat, float radians) {
    float s = (float) sin((double) radians);
    float c = (float) cos((double) radians);

    mat4 r = {{   c, 0.0f,    s, 0.0f,
               0.0f, 1.0f, 0.0f, 0.0f,
                 -s, 0.0f,    c, 0.0f,
               0.0f, 0.0f, 0.0f, 1.0f}};

    mat4_multiply(mat, &r);
}

void mat4_rotate_z(mat4 *mat, float radians) {
    float s = (float) sin((double) radians);
    float c = (float) cos((double) radians);

    mat4 r = {{   c,   -s, 0.0f, 0.0f,
                  s,    c, 0.0f, 0.0f,
               0.0f, 0.0f, 1.0f, 0.0f,
               0.0f, 0.0f, 0.0f, 1.0f}};
    
    mat4_multiply(mat, &r);
}

void mat4_scale(mat4 *mat, float x, float y, float z) {
    mat4 r = {{   x, 0.0f, 0.0f, 0.0f,
               0.0f,    y, 0.0f, 0.0f,
               0.0f, 0.0f,    z, 0.0f,
               0.0f, 0.0f, 0.0f, 1.0f}};

    mat4_multiply(mat, &r);
}

// Camera

void camera_ortho(mat4 *mat, float left, float right, float top, float bottom, float near, float far) {
    mat->data[0]  = 2.0f/(right - left);
    mat->data[5]  = 2.0f/(top - bottom);
    mat->data[10] = 2.0f/(far - near);
    mat->data[12] = -((right + left)/(right - left));
    mat->data[13] = -((top + bottom)/(top - bottom));
    mat->data[14] = -((far + near)/(far - near));
}

void camera_persp(mat4 *mat, float fov_y, float aspect, float near, float far) {
    float h = 2.0f * near * tanf(fov_y/2);
    float w = aspect * h;

    mat->data[0]  = (2.0f * near) / w;
    mat->data[5]  = (2.0f * near) / h;
    mat->data[10] = -((far + near) / (far - near));
    mat->data[11] = -((2 * far * near)/(far - near));
    mat->data[14] = -1.0f;
    mat->data[15] = 0.0f;
}

/*
int main(void) {
    printf("mat3 printing\n");
    matrix3f test = {{0.0f, 0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f}};
  
    set_identity_3f(&test);
    print_matrix_3f(&test);

    printf("\n");
    translate_matrix_3f(&test, 2.0f, 1.0f);
    rotate_matrix_3f(&test, degrees_to_radians(45.0f));
    scale_matrix_3f(&test, 0.5f, 0.5f);
    print_matrix_3f(&test);

    printf("\n");
    set_identity_3f(&test);
    transform_matrix_3f(&test, 2.0f, 1.0f, 45.0f, 0.5f, 0.5f);
    print_matrix_3f(&test);

    // mat4
    printf("mat4 printing\n");
    matrix4f test2;
    set_identity_4f(&test2);
    print_matrix_4f(&test2);
    printf("\n");

    translate_matrix_4f(&test2, 2.0f, 1.0f, -1.0f);
    rotate_matrix_4f_x(&test2, degrees_to_radians(45.0f));
    scale_matrix_4f(&test2, 0.5f, 0.5f, 0.5f);
    print_matrix_4f(&test2);

    return 0;
}
*/
