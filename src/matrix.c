#include <stdio.h>
#include <math.h>
#include "matrix.h"

#define _USE_MATH_DEFINES

void set_identity_nf(float data[], int n) {
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

void multiply_matrix_nf(float *dest, float *with, int n) {
    // float *result;  // issue here, pointer to unknown

    float result[n * n];
    set_identity_nf(result, n);

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

void set_identity_3f(matrix3f *mat) {
    set_identity_nf(mat->data, 3);
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

void print_matrix_3f(matrix3f *mat) {
    print_matrix_n(mat->data, 3);
}

void translate_matrix_3f(matrix3f *mat, float x, float y) {
    matrix3f t = {{1.0f, 0.0f, x, 0.0f, 1.0f, y, 0.0f, 0.0f, 1.0f}};
    multiply_matrix_3f(mat, &t);
}

void multiply_matrix_3f(matrix3f *dest, matrix3f *with) {
    multiply_matrix_nf(dest->data, with->data, 3);
}

void rotate_matrix_3f(matrix3f *mat, float radians) {
    float s = (float) sin((double) radians);
    float c = (float) cos((double) radians);

    matrix3f r = {{c, s, 0.0f, -s, c, 0.0f, 0.0f, 0.0f, 1.0f}};

    multiply_matrix_3f(mat, &r);
}

void scale_matrix_3f(matrix3f *mat, float x, float y) {
    matrix3f s = {{x, 0.0f, 0.0f, 0.0f, y, 0.0f, 0.0f, 0.0f, 1.0f}};    
    multiply_matrix_3f(mat, &s);
}

float degrees_to_radians(float degrees) {
    return degrees * (M_PI/180);
}

void transform_matrix_3f(matrix3f *mat, float x, float y, float r_degrees, float sx, float sy) {
    translate_matrix_3f(mat, x, y);
    rotate_matrix_3f(mat, degrees_to_radians(r_degrees));
    scale_matrix_3f(mat, sx, sy);
}

void set_identity_4f(matrix4f *mat) {
    set_identity_nf(mat->data, 4);
}

void print_matrix_4f(matrix4f *mat) {
    print_matrix_n(mat->data, 4);
}

void translate_matrix_4f(matrix4f *mat, float x, float y, float z) {
    matrix4f t = {{1.0f, 0.0f, 0.0f, x,
                   0.0f, 1.0f, 0.0f, y,
                   0.0f, 0.0f, 1.0f, z,
                   0.0f, 0.0f, 0.0f, 1.0f}};

    multiply_matrix_4f(mat, &t);
}

void multiply_matrix_4f(matrix4f *dest, matrix4f *with) {
    multiply_matrix_nf(dest->data, with->data, 4);
}

void rotate_matrix_4f_x(matrix4f *mat, float radians) {
    float s = (float) sin((double) radians);
    float c = (float) cos((double) radians);

    // matrix4f r;
    // set_identity_4f(&r);
    // r.data[5] = c;
    // r.data[6] = -s;
    // r.data[9] = s;
    // r.data[10] = c;
    matrix4f r = {{1.0f, 0.0f, 0.0f, 0.0f,
                   0.0f,    c,   -s, 0.0f,
                   0.0f,    s,    c, 0.0f,
                   0.0f, 0.0f, 0.0f, 1.0f}};

    multiply_matrix_4f(mat, &r);
}

// TODO: remove identity setting. Set temp mat directly to correct mat values.
void rotate_matrix_4f_y(matrix4f *mat, float radians) {
    float s = (float) sin((double) radians);
    float c = (float) cos((double) radians);

    // matrix4f r;
    // set_identity_4f(&r);
    // r.data[0] = c;
    // r.data[2] = s;
    // r.data[8] = -s;
    // r.data[10] = c;
    matrix4f r = {{   c, 0.0f,    s, 0.0f,
                   0.0f, 1.0f, 0.0f, 0.0f,
                     -s, 0.0f,    c, 0.0f,
                   0.0f, 0.0f, 0.0f, 1.0f}};

    multiply_matrix_4f(mat, &r);
}

void rotate_matrix_4f_z(matrix4f *mat, float radians) {
    float s = (float) sin((double) radians);
    float c = (float) cos((double) radians);

    matrix4f r;
    set_identity_4f(&r);
    r.data[0] = c;
    r.data[1] = -s;
    r.data[4] = s;
    r.data[5] = c;

    multiply_matrix_4f(mat, &r);
}

void scale_matrix_4f(matrix4f *mat, float x, float y, float z) {
    matrix4f r;
    set_identity_4f(&r);
    r.data[0] = x;
    r.data[5] = y;
    r.data[10] = z;

    multiply_matrix_4f(mat, &r);
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
