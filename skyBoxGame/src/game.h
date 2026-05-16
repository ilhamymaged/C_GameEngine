#pragma once

#include <defines.h>
#include <game_types.h>
#include "renderer/camera.h"
#include "renderer/opengl/shader.h"
#include "renderer/opengl/model.h"
#include "renderer/opengl/material.h"
#include "renderer/opengl/texture.h"
#include "renderer/ui/ui.h"

typedef struct game_state {
    f32 delta_time;

    ui_context ui;

    camera game_camera;
    
    shader game_shader;
    shader light_shader;
    
    model light_cube;
    model light_red_cube;
    model backpack_model;
    model grid;
    
    texture backpack_texture;
    
    material backpack_material;
    
    model_instance backpack_instance;
}game_state;

b8 game_init(game* game_inst);
b8 game_update(game* game_inst, f32 delta_time);
b8 game_render(game* game_inst, f32 delta_time);
b8 game_ui_render(game* game_inst, f32 delta_time);
void game_on_resize(game* game_inst, u32 width, u32 height);
void game_shutdown(game* game_inst);
