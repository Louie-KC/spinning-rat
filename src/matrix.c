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
    // mat->data[2] += x;
    // mat->data[5] += y;
    multiply_matrix_3f(mat, &t);
}

void multiply_matrix_3f(matrix3f *dest, matrix3f *with) {
    matrix3f result;

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            unsigned int idx = 3 * i + j;
            result.data[idx] = 0;
            for (int k = 0; k < 3; k++) {
                result.data[idx] += dest->data[3 * i + k] * with->data[3 * k + j];
            }
        }
    }
    // copy result to dest
    unsigned int i = 0;
    while (i < 9) {
        dest->data[i] = result.data[i];
        i++;
    }
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

int main(void) {
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

    return 0;
}
