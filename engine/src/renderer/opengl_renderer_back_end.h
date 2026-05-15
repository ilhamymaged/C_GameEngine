#pragma once

#include "defines.h"
#include "model.h"
#include "shader.h"

void renderer_back_end_init();
void renderer_back_end_shutdown();

void renderer_back_end_clear_screen(float r, float g, float b, float a);
void renderer_back_end_on_resize(u32 width, u32 height); 

void debug_check(const char* msg);

//Shaders
void renderer_back_end_create_shader(shader* shader, const char* vertex_source, const char* fragment_source);
void renderer_back_end_use_shader(shader* shader);
void renderer_back_end_destroy_shader(shader* shader);
void renderer_back_end_set_uniform_mat4(shader* shader, const char* name, mat4 value);
void renderer_back_end_set_uniform_vec3(shader* shader, const char* name, vec3 value);
void renderer_back_end_set_uniform_float(shader* shader, const char* name, float value);
void renderer_back_end_set_uniform_int(shader* shader, const char* name, int value);
void renderer_back_end_set_uniform_vec4(shader* shader, const char* name, float x, float y, float z, float w);
void renderer_back_end_set_uniform_vec4f(shader* shader, const char* name,vec4 value);
void renderer_back_end_debug_shader_uniforms(shader* shader);

//Models
void renderer_back_end_init_model(model* m, const char* file_path);
void renderer_back_end_draw_model(model* m);
void renderer_back_end_destroy_model(model* m);
void renderer_back_end_debug_model_gpu_status(model* m, const char* model_name);

//Meshes
void renderer_back_end_init_mesh(mesh* m, float *vertices, u64 vertex_count, u32* indices, u64 index_count);
void renderer_back_end_draw_mesh(mesh* m);
void renderer_back_end_destroy_mesh(mesh* m);

