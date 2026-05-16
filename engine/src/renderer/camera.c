#include "camera.h"

#include "core/input.h"
#include <math.h>

void camera_init(camera* camera, float fov_radians, float aspect, float near_plane, float far_plane, vec3 position, vec3 target, vec3 up) {
    if (!camera) {
        return;
    }
    camera->position = position;
    camera->front = target;
    camera->up = up;
    camera->fov = fov_radians;
    camera->aspect = aspect;
    camera->near_plane = near_plane;
    camera->far_plane = far_plane;

    // TODO: change this
    camera->yaw = -90.0f;
    camera->pitch = 0.0f;

    camera_update_view(camera);
    camera_update_projection(camera, aspect);
}

void camera_update_projection(camera* camera, float aspect) {
    if (!camera)
        return;

    camera->aspect = aspect;
    camera->projection = mat4_perspective(camera->fov, camera->aspect, camera->near_plane, camera->far_plane);
}

void camera_update_view(camera* camera) {
    if (!camera) {
        return;
    }
    camera->view = mat4_look_at(camera->position, vec3_add(camera->front, camera->position), camera->up);
}

void camera_move(camera* cam, f32 dt)
{
    const f32 speed = 5.0f;

    vec3 forward = cam->front;
    vec3 right   = vec3_normalize(vec3_cross(cam->front, cam->up));

    if (input_is_key_down(KEY_W)) {
        cam->position = vec3_add(cam->position, vec3_scale(forward, speed * dt));
    }

    if (input_is_key_down(KEY_S)) {
        cam->position = vec3_sub(cam->position, vec3_scale(forward, speed * dt));
    }

    if (input_is_key_down(KEY_A)) {
        cam->position = vec3_sub(cam->position, vec3_scale(right, speed * dt));
    }

    if (input_is_key_down(KEY_D)) {
        cam->position = vec3_add(cam->position, vec3_scale(right, speed * dt));
    }

    if (input_is_key_down(KEY_Q)) {
        cam->position = vec3_sub(cam->position, vec3_scale(cam->up, speed * dt));
    }

    if (input_is_key_down(KEY_E)) {
        cam->position = vec3_add(cam->position, vec3_scale(cam->up, speed * dt));
    }

    // Mouse Movement
    if (input_is_key_down(KEY_UP)) {
        cam->pitch += 50.0f * dt; // Adjust sensitivity as needed
    }

    if (input_is_key_down(KEY_DOWN)) {
        cam->pitch -= 50.0f * dt; // Adjust sensitivity as needed
    }

    if (input_is_key_down(KEY_LEFT)) {
        cam->yaw -= 50.0f * dt; // Adjust sensitivity as needed
    }

    if (input_is_key_down(KEY_RIGHT)) {
        cam->yaw += 50.0f * dt; // Adjust sensitivity as needed
    }

    vec3 direction;
    direction.x = cosf(cam->yaw * 0.01745329251f)
                * cosf(cam->pitch * 0.01745329251f);
    direction.y = sinf(cam->pitch * 0.01745329251f);
    direction.z = sinf(cam->yaw * 0.01745329251f)
                * cosf(cam->pitch * 0.01745329251f);

    cam->front = vec3_normalize(direction);
    camera_update_view(cam);
}