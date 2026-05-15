#include "camera.h"

void camera_init(camera* camera, float fov_radians, float aspect, float near_plane, float far_plane, vec3 position, vec3 target, vec3 up) {
    if (!camera) {
        return;
    }

    camera->position = position;
    camera->target = target;
    camera->up = up;
    camera->fov = fov_radians;
    camera->aspect = aspect;
    camera->near_plane = near_plane;
    camera->far_plane = far_plane;

    camera_update_view(camera);
    camera_update_projection(camera, aspect);
}

void camera_update_projection(camera* camera, float aspect) {
    if (!camera) {
        return;
    }

    camera->aspect = aspect;
    camera->projection = mat4_perspective(camera->fov, camera->aspect, camera->near_plane, camera->far_plane);
}

void camera_update_view(camera* camera) {
    if (!camera) {
        return;
    }

    camera->view = mat4_look_at(camera->position, camera->target, camera->up);
}
