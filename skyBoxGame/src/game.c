#include "game.h"
#include <core/input.h>

#include <core/logger.h>
#include "renderer/renderer.h"
#include "core/AGmemory.h"
#include "utility/math.h"
#include <core/asserts.h>

#define DEG2RAD(x) ((x) * 0.01745329251f)

b8 game_init(game* game_inst) {
    game_state* state = (game_state*)game_inst->state;
    camera_init(&(state->game_camera), 
        45.0f, (float) game_inst->app_config.start_width / (float)game_inst->app_config.start_height
        , 0.1f, 100.0f, (vec3){0, 2, 4}, (vec3){0, 0, -1}, (vec3){0, 1, 0});

    AG_ASSERT_MSG(create_shader(&state->game_shader, "../engine/shaders/basic.vs", "../engine/shaders/basic.frag") == TRUE, 
    "Failed to create game shader.");
    AG_ASSERT_MSG(create_shader(&state->light_shader, "../engine/shaders/light.vs", "../engine/shaders/light.frag") == TRUE,
    "Failed to create light shader.");

    init_model(&state->backpack_model, "../engine/assets/models/backpack/backpack.obj", MODEL_FILE, NULL);

    grid_desc grid = {0};
    init_model(&state->grid, NULL, GRID, &grid);

    sphere_desc light_sphere_config = {0};
    init_model(&state->light_cube, NULL, SPHERE, &light_sphere_config);
    init_model(&state->light_red_cube, NULL, SPHERE, &light_sphere_config);

    AG_ASSERT_MSG(load_texture_from_file("../engine/assets/models/backpack/diffuse.png", &state->backpack_texture) == TRUE,
    "Failed to load backpack texture.");

    init_material(&state->backpack_material, &state->game_shader, &state->backpack_texture, (vec3){1.0f, 1.0f, 1.0f}, TRUE);

    init_model_instance(&state->backpack_instance, &state->backpack_model, &state->backpack_material);

    //UI
    ui_init(&state->ui, game_inst->app_config.start_width, game_inst->app_config.start_height);

    return TRUE;
}

b8 game_update(game* game_inst, f32 delta_time) {
    game_state* state = (game_state*)game_inst->state;
    camera_move(&state->game_camera, delta_time);
    return TRUE;
}

b8 game_render(game* game_inst, f32 delta_time) {
    game_state* state = (game_state*)game_inst->state;

    use_shader(&state->light_shader);

    vec3 lightColor0 = {1.0f, 1.0f, 1.0f};
    vec3 lightColor1 = {1.0f, 0.0f, 1.0f};
    vec3 lightPos0 = {5.0f, 5.0f, 0.0f};
    vec3 lightPos1 = {5.0f, 10.0f, -2.0f};
    float lightRadius0 = 25.0f;
    float lightRadius1 = 10.0f;

    mat4 model = get_model_matrix(lightPos0, (vec3){0.2, 0.2, 0.2}, (vec3){90, 0, 0});
    set_shader_mat4(&state->light_shader, "uView", state->game_camera.view);
    set_shader_mat4(&state->light_shader, "uProjection", state->game_camera.projection);
 
    set_shader_mat4(&state->light_shader, "uModel", model);
    set_shader_vec3(&state->light_shader, "uLightColor", lightColor0);
    draw_model(&state->light_cube);

    model = get_model_matrix(lightPos1, (vec3){0.2, 0.2, 0.2}, (vec3){90, 90, 0});
    set_shader_mat4(&state->light_shader, "uModel", model);
    set_shader_vec3(&state->light_shader, "uLightColor", lightColor1);
    draw_model(&state->light_red_cube);

    use_shader(&state->game_shader);

    set_shader_mat4(&state->game_shader, "uView", state->game_camera.view);
    set_shader_mat4(&state->game_shader, "uProjection", state->game_camera.projection);
    set_shader_vec3(&state->game_shader, "uViewPos", state->game_camera.position);

    set_shader_vec3(&state->game_shader, "uLightColor[0]", lightColor0);
    set_shader_vec3(&state->game_shader, "uLightColor[1]", lightColor1);
    set_shader_vec3(&state->game_shader, "uLightPos[0]", lightPos0);
    set_shader_vec3(&state->game_shader, "uLightPos[1]", lightPos1);
    set_shader_float(&state->game_shader, "uLightRadius[0]", lightRadius0);
    set_shader_float(&state->game_shader, "uLightRadius[1]", lightRadius1);
    set_shader_int(&state->game_shader, "uActiveLightCount", 2);

    model = get_model_matrix((vec3){1.0, 0.5, 0}, (vec3){1.0, 1.0, 1.0}, (vec3){0, 0, 0});
    set_shader_mat4(&state->game_shader, "uModel", model);
    draw_model_instance(&state->backpack_instance);

    model = get_model_matrix((vec3){0, 0, 0}, (vec3){1.0, 1.0, 1.0}, (vec3){0, 0, 0});
    set_shader_mat4(&state->game_shader, "uModel", model);
    set_shader_int(&state->game_shader, "uUseAlbedo", 0);
    draw_model_lines(&state->grid);

    return TRUE;
}

b8 game_ui_render(game* game_inst, f32 delta_time) {
    game_state* state = (game_state*)game_inst->state;
    ui_context* ui = &state->ui; 
    ui_begin(ui);

    ui_draw_rect(ui, 100, 100, 10, 10);
    ui_draw_rect(ui, 400, 200, 10, 10);

    

    return TRUE;
}

void game_on_resize(game* game_inst, u32 width, u32 height) {
    game_state* state = (game_state*)game_inst->state;
    camera_update_projection(&state->game_camera, (float)width / (float)height);
}

void game_shutdown(game* game_inst) {
    game_state* state = (game_state*)game_inst->state;
    
    destroy_shader(&state->game_shader);
    destroy_shader(&state->light_shader);
    
    destroy_model(&state->light_cube);
    destroy_model(&state->backpack_model);
    destroy_model(&state->grid);
    destroy_model(&state->light_red_cube);

    destroy_texture(&state->backpack_texture);

    ui_shutdown(&state->ui);

    AGfree(game_inst->state, sizeof(game_state), MEMORY_TAG_GAME);
}
