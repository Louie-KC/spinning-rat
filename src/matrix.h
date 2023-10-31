typedef struct matrix3f {
    float data[9];
} matrix3f;

 void set_identity_nf(float data[], int n);
 void set_identity_3f(matrix3f *mat);
 void print_matrix_n(float data[], int n);
 void print_matrix_3f(matrix3f *mat);
 void translate_matrix_3f(matrix3f *mat, float x, float y);
 void multiply_matrix_3f(matrix3f *dest, matrix3f *with);
 void rotate_matrix_3f(matrix3f *mat, float radians);
 void scale_matrix_3f(matrix3f *mat, float x, float y);
 float degrees_to_radians(float degrees);
 void transform_matrix_3f(matrix3f *mat, float x, float y, float r_degrees, float sx, float sy);
