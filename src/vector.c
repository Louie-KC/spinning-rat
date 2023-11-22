#include "vector.h"

vec2 vec2_zero() {
    vec2 result = {0.0f, 0.0f};
    return result;
}

void vec2_set_zero(vec2 *vec) {
    vec->x = 0.0f;
    vec->y = 0.0f;
}

void vec2_negate(vec2 *vec) { 
    vec->x *= -1.0f;
    vec->y *= -1.0f;
}

void vec2_add(vec2 *dest, vec2 *with) {
    dest->x += with->x;
    dest->y += with->y;
}

void vec2_sub(vec2 *dest, vec2 *with) {
    dest->x -= with->x;
    dest->y -= with->y;
}

float vec2_magnitude(vec2 vec) {
    float x = vec.x;
    float y = vec.y;
    return (float) sqrt(x*x + y*y);
}

void vec2_normalise(vec2 *vec) {
    float magnitude = vec2_magnitude(*vec);
    vec->x /= magnitude;
    vec->y /= magnitude;
}

float vec2_dot_product(vec2 a, vec2 b) {
    float result = 0.0f;
    result += a.x * b.x;
    result += a.y * b.y;
    return result;
}

vec3 vec3_zero() {
    vec3 result = {0.0f, 0.0f, 0.0f};
    return result;
}

void vec3_set_zero(vec3 *vec) {
    vec->x = 0.0f;
    vec->y = 0.0f;
    vec->z = 0.0f;
}

void vec3_negate(vec3 *vec) {
    vec->x *= -1.0f;
    vec->y *= -1.0f;
    vec->z *= -1.0f;
}

void vec3_add(vec3 *dest, vec3 *with) {
    dest->x += with->x;
    dest->y += with->y;
    dest->z += with->z;
}

void vec3_sub(vec3 *dest, vec3 *with) {
    dest->x -= with->x;
    dest->y -= with->y;
    dest->z -= with->z;
}

float vec3_magnitude(vec3 vec) {
    float x = vec.x;
    float y = vec.y;
    float z = vec.z;
    return (float) sqrt(x*x + y*y + z*z);
}

void vec3_normalise(vec3 *vec) {
    float magnitude = vec3_magnitude(*vec);
    vec->x /= magnitude;
    vec->y /= magnitude;
    vec->z /= magnitude;
}

float vec3_dot_product(vec3 a, vec3 b) {   
    float result = 0;
    result += a.x * b.x;
    result += a.y * b.y;
    result += a.z * b.z;
    return result;
}

vec4 vec4_zero() {
    vec4 result = {0.0f, 0.0f, 0.0f, 0.0f};
    return result;
}

void vec4_set_zero(vec4 *vec) {
    vec->x = 0.0f;
    vec->y = 0.0f;
    vec->z = 0.0f;
    vec->w = 0.0f;
}

void vec4_negate(vec4 *vec) {
    vec->x *= -1.0f;
    vec->y *= -1.0f;
    vec->z *= -1.0f;
    vec->w *= -1.0f;
}

void vec4_add(vec4 *dest, vec4 *with) {
    dest->x += with->x;
    dest->y += with->y;
    dest->z += with->z;
    dest->w += with->w;
}

void vec4_sub(vec4 *dest, vec4 *with) {
    dest->x -= with->x;
    dest->y -= with->y;
    dest->z -= with->z;
    dest->w -= with->w;
}

float vec4_magnitude(vec4 vec) {
    float x = vec.x;
    float y = vec.y;
    float z = vec.z;
    float w = vec.w;
    return (float) sqrt(x*x + y*y + z*z + w*w);
}

void vec4_normalise(vec4 *vec) {
    float magnitude = vec4_magnitude(*vec);
    vec->x /= magnitude;
    vec->y /= magnitude;
    vec->z /= magnitude;
    vec->w /= magnitude;
}

float vec4_dot_product(vec4 a, vec4 b) {
    float result = 0;
    result += a.x * b.x;
    result += a.y * b.y;
    result += a.z * b.z;
    result += a.w * b.w;
    return result;
}
