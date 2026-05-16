#include "math.h"
#include <math.h>

mat4 mat4_identity() {
    mat4 result = {0};
    result.elements[0] = 1.0f;
    result.elements[5] = 1.0f;
    result.elements[10] = 1.0f;
    result.elements[15] = 1.0f;
    return result;
}

mat4 mat4_translate(vec3 t) {
    mat4 result = mat4_identity();

    result.elements[12] = t.x;
    result.elements[13] = t.y;
    result.elements[14] = t.z;

    return result;
}

mat4 mat4_scale(vec3 s) {
    mat4 r = mat4_identity();

    r.elements[0]  = s.x;
    r.elements[5]  = s.y;
    r.elements[10] = s.z;

    return r;
}

mat4 mat4_rotate_y(float angle) {
    mat4 r = mat4_identity();

    float c = cosf(angle);
    float s = sinf(angle);

    r.elements[0]  = c;
    r.elements[2]  = s;
    r.elements[8]  = -s;
    r.elements[10] = c;

    return r;
}

mat4 mat4_rotate_x(float angle) {
    mat4 r = mat4_identity();

    float c = cosf(angle);
    float s = sinf(angle);

    r.elements[5]  = c;
    r.elements[6]  = s;
    r.elements[9]  = -s;
    r.elements[10] = c;

    return r;
}

mat4 mat4_rotate_z(float angle) {
    mat4 r = mat4_identity();

    float c = cosf(angle);
    float s = sinf(angle);

    r.elements[0] = c;
    r.elements[1] = -s;
    r.elements[4] = s;
    r.elements[5] = c;

    return r;
}

mat4 get_model_matrix(vec3 pos, vec3 scale, vec3 rotation) {
    mat4 T = mat4_translate(pos);

    mat4 Rx = mat4_rotate_x(rotation.x);
    mat4 Ry = mat4_rotate_y(rotation.y);
    mat4 Rz = mat4_rotate_z(rotation.z);

    mat4 R = mat4_multiply(Rz, mat4_multiply(Ry, Rx));

    mat4 S = mat4_scale(scale);

    return mat4_multiply(T, mat4_multiply(R, S));
}

mat4 mat4_perspective(float fov_radians, float aspect, float near_plane, float far_plane) {
    mat4 result = {0};
    float f = 1.0f / tanf(fov_radians * 0.5f);
    result.elements[0] = f / aspect;
    result.elements[5] = f;
    result.elements[10] = (far_plane + near_plane) / (near_plane - far_plane);
    result.elements[11] = -1.0f;
    result.elements[14] = (2.0f * far_plane * near_plane) / (near_plane - far_plane);
    return result;
}

mat4 mat4_ortho(float left, float right, float bottom, float top, float near_plane, float far_plane) {
    mat4 r = {0};

    r.elements[0]  = 2.0f / (right - left);
    r.elements[5]  = 2.0f / (top - bottom);
    r.elements[10] = -2.0f / (far_plane - near_plane);

    r.elements[12] = -(right + left) / (right - left);
    r.elements[13] = -(top + bottom) / (top - bottom);
    r.elements[14] = -(far_plane + near_plane) / (far_plane - near_plane);
    r.elements[15] = 1.0f;

    return r;
}

mat4 mat4_look_at(vec3 eye, vec3 center, vec3 up) {
    vec3 f = vec3_normalize(vec3_sub(center, eye));
    vec3 s = vec3_normalize(vec3_cross(f, up));
    vec3 u = vec3_cross(s, f);

    mat4 result = mat4_identity();
    result.elements[0] = s.x;
    result.elements[1] = u.x;
    result.elements[2] = -f.x;

    result.elements[4] = s.y;
    result.elements[5] = u.y;
    result.elements[6] = -f.y;

    result.elements[8] = s.z;
    result.elements[9] = u.z;
    result.elements[10] = -f.z;

    result.elements[12] = -vec3_dot(s, eye);
    result.elements[13] = -vec3_dot(u, eye);
    result.elements[14] = vec3_dot(f, eye);
    return result;
}

mat4 mat4_multiply(mat4 a, mat4 b) {
    mat4 result = {0};

    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 4; ++col) {
            float sum = 0.0f;
            for (int i = 0; i < 4; ++i) {
                sum += a.elements[i * 4 + row] * b.elements[col * 4 + i];
            }
            result.elements[col * 4 + row] = sum;
        }
    }

    return result;
}

vec3 vec3_add(vec3 a, vec3 b) {
    return (vec3){a.x + b.x, a.y + b.y, a.z + b.z};
}

vec3 vec3_scale(vec3 v, float scalar) {
    return (vec3){v.x * scalar, v.y * scalar, v.z * scalar};
}

float vec3_length(vec3 v) {
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

vec3 vec3_normalize(vec3 v) {
    float len = vec3_length(v);
    if (len == 0.0f) {
        return (vec3){0.0f, 0.0f, 0.0f};
    }
    return (vec3){v.x / len, v.y / len, v.z / len};
}

vec3 vec3_sub(vec3 a, vec3 b) {
    return (vec3){a.x - b.x, a.y - b.y, a.z - b.z};
}

vec3 vec3_cross(vec3 a, vec3 b) {
    return (vec3){
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

float vec3_dot(vec3 a, vec3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

float vec4_length(vec4 v) {
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
}

vec4 vec4_normalize(vec4 v) {
    float len = vec4_length(v);
    if (len == 0.0f) {
        return (vec4){0.0f, 0.0f, 0.0f, 0.0f};
    }
    return (vec4){v.x / len, v.y / len, v.z / len, v.w / len};
}

vec4 vec4_sub(vec4 a, vec4 b) {
    return (vec4){a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w};
}

vec4 vec4_cross(vec4 a, vec4 b) {
    return (vec4){
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x,
        0.0f
    };
}

float vec4_dot(vec4 a, vec4 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}
