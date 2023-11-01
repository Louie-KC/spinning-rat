#include <math.h>

typedef struct vector2f {
    float x;
    float y;
} vector2f;

typedef struct vector3f {
    float x;
    float y;
    float z;
} vector3f;

typedef struct vector4f {
    float x;
    float y;
    float z;
    float w;
} vector4f;

void reset_vec2f(vector2f *vec) {
    vec->x = 0.0f;
    vec->y = 0.0f;
}

void reset_vec3f(vector3f *vec) {
    vec->x = 0.0f;
    vec->y = 0.0f;
    vec->z = 0.0f;
}

void reset_vec4f(vector4f *vec) {
    vec->x = 0.0f;
    vec->y = 0.0f;
    vec->z = 0.0f;
    vec->w = 0.0f;
}

void negate_vec2f(vector2f *vec) { 
    vec->x *= -1.0f;
    vec->y *= -1.0f;
}

void negate_vec3f(vector3f *vec) {
    vec->x *= -1.0f;
    vec->y *= -1.0f;
    vec->z *= -1.0f;
}

float magnitude_vec2f(vector2f vec) {
    float x = vec.x;
    float y = vec.y;
    return (float) sqrt(x*x + y*y);
}

float magnitude_vec3f(vector3f vec) {
    float x = vec.x;
    float y = vec.y;
    float z = vec.z;
    return (float) sqrt(x*x + y*y + z*z); 
}

void normalise_vec2f(vector2f *vec) {
    float magnitude = magnitude_vec2f(*vec);
    vec->x /= magnitude;
    vec->y /= magnitude;
}

void normalise_vec3f(vector3f *vec) {
    float magnitude = magnitude_vec3f(*vec);
    vec->x /= magnitude; 
    vec->y /= magnitude; 
    vec->z /= magnitude; 
}

vector2f add_vec2f(vector2f a, vector2f b) {
    a.x += b.x;
    a.y += b.y;
    return a;
}

vector3f add_vec3f(vector3f a, vector3f b) {
    a.x += b.x;
    a.y += b.y;
    a.z += b.z;
    return a;
}

vector2f sub_vec2f(vector2f a, vector2f b) {
    a.x -= b.x;
    a.y -= b.y;
    return a;
}

vector3f sub_vec3f(vector3f a, vector3f b) {
    a.x -= b.x;
    a.y -= b.y;
    a.z -= b.z;
    return a;
}

float dot_product_vec2f(vector2f a, vector3f b) {
    float result = 0;
    result += a.x * b.x;
    result += a.y * b.y;
    return result;
}

float dot_product_vec3f(vector3f a, vector3f b) {   
    float result = 0;
    result += a.x * b.x;
    result += a.y * b.y;
    result += a.z * b.z;
    return result;
}
