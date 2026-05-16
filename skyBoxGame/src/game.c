#include "game.h"
#include <core/input.h>

#include <core/logger.h>
#include "renderer/renderer.h"
#include "core/AGmemory.h"
#include "utility/math.h"

#define DEG2RAD(x) ((x) * 0.01745329251f)

b8 game_init(game* game_inst) {
    game_state* state = (game_state*)game_inst->state;
    camera_init(&(state->game_camera), 
        45.0f, (float) game_inst->app_config.start_width / (float)game_inst->app_config.start_height
        , 0.1f, 100.0f, (vec3){0, 0, 4}, (vec3){0, 0, -1}, (vec3){0, 1, 0});

    create_shader(&state->game_shader, "../engine/shaders/basic.vs", "../engine/shaders/basic.frag");
    init_model(&state->backpack_model, "../engine/assets/models/backpack/backpack.obj");
    load_texture_from_file("../engine/assets/models/backpack/diffuse.png", &state->backpack_texture);
    init_material(&state->backpack_material, &state->game_shader, &state->backpack_texture, (vec3){1.0f, 1.0f, 1.0f}, TRUE);
    init_model_instance(&state->backpack_instance, &state->backpack_model, &state->backpack_material);

    return TRUE;
}

b8 game_update(game* game_inst, f32 delta_time) {
    game_state* state = (game_state*)game_inst->state;
    camera_move(&state->game_camera, delta_time);
    return TRUE;
}

b8 game_render(game* game_inst, f32 delta_time) {
    game_state* state = (game_state*)game_inst->state;

    use_shader(&state->game_shader);

    mat4 model = mat4_identity();
    set_shader_mat4(&state->game_shader, "uModel", model);
    set_shader_mat4(&state->game_shader, "uView", state->game_camera.view);
    set_shader_mat4(&state->game_shader, "uProjection", state->game_camera.projection);

    draw_model_instance(&state->backpack_instance);
    return TRUE;
}

void game_on_resize(game* game_inst, u32 width, u32 height) {
    game_state* state = (game_state*)game_inst->state;
    camera_update_projection(&state->game_camera, (float)width / (float)height);
}

void game_shutdown(game* game_inst) {
    game_state* state = (game_state*)game_inst->state;
    destroy_shader(&state->game_shader);
    destroy_model(&state->backpack_model);
    destroy_texture(&state->backpack_texture);
    AGfree(game_inst->state, sizeof(game_state), MEMORY_TAG_GAME);
}
