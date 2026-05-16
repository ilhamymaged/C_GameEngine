#pragma once

#include "utility/math.h"

typedef struct camera {
    vec3 position;
    vec3 front;
    vec3 up;
    mat4 view;
    mat4 projection;
    float fov;
    float aspect;
    float near_plane;
    float far_plane;
    float yaw;
    float pitch;
} camera;

AGINA_API void camera_init(camera* camera, float fov_radians, float aspect, float near_plane, float far_plane, vec3 position, vec3 target, vec3 up);
AGINA_API void camera_update_projection(camera* camera, float aspect);
AGINA_API void camera_update_view(camera* camera);
AGINA_API void camera_move(camera* cam, f32 dt);
