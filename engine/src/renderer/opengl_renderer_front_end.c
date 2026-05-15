#include "opengl_renderer_front_end.h"
#include "opengl_renderer_back_end.h"

#include "shader.h"
#include "camera.h"

#include <glad/glad.h>
#include <core/logger.h>

#include "model.h"

static platform_state* state;

typedef struct renderer_state {
    u32 width;
    u32 height;
    shader test_shader;
    camera camera;
    model backpack_model;
    // triangle test_triangle;
}renderer_state;

struct renderer_state r_state;

void opengl_renderer_init(platform_state* plat_state, u32 width, u32 height) {
    renderer_back_end_init();
    state = plat_state;
    r_state.width = width;
    r_state.height = height;

    camera_init(&r_state.camera, 
        (float)(45.0f), width / (float)height, 0.1f, 100.0f, (vec3){0, 0, 3}, (vec3){0, 0, 0}, (vec3){0, 1, 0});

    renderer_back_end_create_shader(&r_state.test_shader, "../engine/shaders/basic.vs", "../engine/shaders/basic.frag");
    renderer_back_end_init_model(&r_state.backpack_model, "../engine/assets/models/backpack/backpack.obj");

    event_register(EVENT_CODE_RESIZED, state, opengl_renderer_on_resize); 
}

void opengl_renderer_shutdown() {
    renderer_back_end_destroy_shader(&r_state.test_shader);
    renderer_back_end_destroy_model(&r_state.backpack_model);
    renderer_back_end_shutdown();

    event_unregister(EVENT_CODE_RESIZED, state, opengl_renderer_on_resize);
}

void opengl_renderer_clear_screen(float r, float g, float b, float a) {
    renderer_back_end_clear_screen(r, g, b, a);
}

void opengl_renderer_begin_frame() {
    renderer_back_end_use_shader(&r_state.test_shader);

    mat4 model_matrix = mat4_identity();
    renderer_back_end_set_uniform_mat4(&r_state.test_shader, "uModel", model_matrix);
    renderer_back_end_set_uniform_mat4(&r_state.test_shader, "uView", r_state.camera.view);
    renderer_back_end_set_uniform_mat4(&r_state.test_shader, "uProjection", r_state.camera.projection);

    renderer_back_end_draw_model(&r_state.backpack_model);
}

b8 opengl_renderer_on_resize(u16 code, void* sender, void* listener_inst, event_context data) {
    (void)listener_inst;
    u32 width = data.data.u32[0];
    u32 height = data.data.u32[1];
    r_state.width = width;
    r_state.height = height;
    renderer_back_end_on_resize(width, height);

    if (height != 0) {
        camera_update_projection(&r_state.camera, width / (float)height);
    }

    return TRUE;
}

void opengl_renderer_end_frame() {
    platform_swap_buffers(state);
}