#include "matrix.h"

void camera_ortho(mat4 *mat, float left, float right, float top, float bottom, float near, float far);
void camera_persp(mat4 *mat, float fov_y, float aspect, float near, float far);