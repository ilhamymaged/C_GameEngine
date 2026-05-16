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

AGINA_API mat4 mat4_identity();

AGINA_API mat4 mat4_perspective(float fov_radians, float aspect, float near_plane, float far_plane);
AGINA_API mat4 mat4_ortho(float left, float right, float bottom, float top, float near_plane, float far_plane);

AGINA_API mat4 mat4_look_at(vec3 eye, vec3 center, vec3 up);

AGINA_API mat4 mat4_multiply(mat4 a, mat4 b);

AGINA_API mat4 mat4_translate(vec3 t);

AGINA_API mat4 mat4_scale(vec3 s);

AGINA_API mat4 mat4_rotate_x(float angle_x);
AGINA_API mat4 mat4_rotate_y(float angle_y);
AGINA_API mat4 mat4_rotate_z(float angle_z);

AGINA_API mat4 get_model_matrix(vec3 pos, vec3 scale, vec3 rotation);

AGINA_API float vec3_length(vec3 v);
AGINA_API vec3 vec3_add(vec3 a, vec3 b);
AGINA_API vec3 vec3_scale(vec3 v, float scalar);
AGINA_API vec3 vec3_normalize(vec3 v);
AGINA_API vec3 vec3_sub(vec3 a, vec3 b);
AGINA_API vec3 vec3_cross(vec3 a, vec3 b);
AGINA_API float vec3_dot(vec3 a, vec3 b);

AGINA_API float vec4_length(vec4 v);
AGINA_API vec4 vec4_normalize(vec4 v);
AGINA_API vec4 vec4_sub(vec4 a, vec4 b);
AGINA_API vec4 vec4_cross(vec4 a, vec4 b);
AGINA_API float vec4_dot(vec4 a, vec4 b);
