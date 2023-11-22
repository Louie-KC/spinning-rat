#ifndef VECTOR_H
#define VECTOR_H

#include <math.h>

typedef struct vector2f {
    float x;
    float y;
} vec2;

typedef struct vector3f {
    float x;
    float y;
    float z;
} vec3;

typedef struct vector4f {
    float x;
    float y;
    float z;
    float w;
} vec4;

vec2 vec2_zero();
void vec2_set_zero(vec2 *vec);
void vec2_negate(vec2 *vec);
void vec2_add(vec2 *dest, vec2 *with);
void vec2_sub(vec2 *dest, vec2 *with);
float vec2_magnitude(vec2 vec);
void vec2_normalise(vec2 *vec);
float vec2_dot_product(vec2 a, vec2 b);

vec3 vec3_zero();
void vec3_set_zero(vec3 *vec);
void vec3_negate(vec3 *vec);
void vec3_add(vec3 *dest, vec3 *with);
void vec3_sub(vec3 *dest, vec3 *with);
float vec3_magnitude(vec3 vec);
void vec3_normalise(vec3 *vec);
float vec3_dot_product(vec3 a, vec3 b);

vec4 vec4_zero();
void vec4_set_zero(vec4 *vec);
void vec4_negate(vec4 *vec);
void vec4_add(vec4 *dest, vec4 *with);
void vec4_sub(vec4 *dest, vec4 *with);
float vec4_magnitude(vec4 vec);
void vec4_normalise(vec4 *vec);
float vec4_dot_product(vec4 a, vec4 b);

#endif