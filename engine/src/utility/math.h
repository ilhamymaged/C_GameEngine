#pragma once

#include "defines.h"

typedef struct vec3 {
    float x;
    float y;
    float z;
} vec3;

typedef struct vec4 {
    float x;
    float y;
    float z;
    float w;
} vec4;

typedef struct mat4 {
    float elements[16];
} mat4;

mat4 mat4_identity();
mat4 mat4_perspective(float fov_radians, float aspect, float near_plane, float far_plane);
mat4 mat4_look_at(vec3 eye, vec3 center, vec3 up);
mat4 mat4_multiply(mat4 a, mat4 b);

float vec3_length(vec3 v);
vec3 vec3_normalize(vec3 v);
vec3 vec3_sub(vec3 a, vec3 b);
vec3 vec3_cross(vec3 a, vec3 b);
float vec3_dot(vec3 a, vec3 b);

float vec4_length(vec4 v);
vec4 vec4_normalize(vec4 v);
vec4 vec4_sub(vec4 a, vec4 b);
vec4 vec4_cross(vec4 a, vec4 b);
float vec4_dot(vec4 a, vec4 b);
