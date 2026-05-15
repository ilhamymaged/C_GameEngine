#pragma once

#include "utility/math.h"

typedef struct camera {
    vec3 position;
    vec3 target;
    vec3 up;
    mat4 view;
    mat4 projection;
    float fov;
    float aspect;
    float near_plane;
    float far_plane;
} camera;

void camera_init(camera* camera, float fov_radians, float aspect, float near_plane, float far_plane, vec3 position, vec3 target, vec3 up);
void camera_update_projection(camera* camera, float aspect);
void camera_update_view(camera* camera);
