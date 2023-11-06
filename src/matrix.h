typedef struct matrix3f {
    float data[9];
} matrix3f;

typedef struct matrix4f {
    float data[16];
} matrix4f;

float degrees_to_radians(float degrees);

void set_identity_nf(float data[], int n);
void set_identity_3f(matrix3f *mat);
void print_matrix_n(float data[], int n);
void print_matrix_3f(matrix3f *mat);
void translate_matrix_3f(matrix3f *mat, float x, float y);
void multiply_matrix_3f(matrix3f *dest, matrix3f *with);
void rotate_matrix_3f(matrix3f *mat, float radians);
void scale_matrix_3f(matrix3f *mat, float x, float y);
void transform_matrix_3f(matrix3f *mat, float x, float y, float r_degrees, float sx, float sy);

void set_identity_4f(matrix4f *mat);
void print_matrix_4f(matrix4f *mat);
void translate_matrix_4f(matrix4f *mat, float x, float y, float z);
void multiply_matrix_4f(matrix4f *dest, matrix4f *with);
void rotate_matrix_4f_x(matrix4f *mat, float radians);
void rotate_matrix_4f_y(matrix4f *mat, float radians);
void rotate_matrix_4f_z(matrix4f *mat, float radians);
void scale_matrix_4f(matrix4f *mat, float x, float y, float z);
