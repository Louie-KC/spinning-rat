#include "camera.h"

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