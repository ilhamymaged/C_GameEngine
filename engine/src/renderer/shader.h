#pragma once

#include "defines.h"
#include <glad/glad.h>
#include "utility/math.h"

#define MAX_SHADER_UNIFORMS 128

typedef struct uniform_cache_entry {
    char name[64];
    GLint location;
} uniform_cache_entry;

typedef struct shader {
    u32 id;
    uniform_cache_entry* uniform_cache;
    u32 uniform_cache_count;
} shader;

b8 create_shader(shader* shader, const char* vertex_source, const char* fragment_source);
b8 use_shader(shader* shader);
b8 destroy_shader(shader* shader);

//UNIFORMS
void set_shader_mat4(shader* shader, const char* name, mat4 value);
void set_shader_vec3(shader* shader, const char* name, vec3 value);
void set_shader_float(shader* shader, const char* name, float value);
void set_shader_int(shader* shader, const char* name, int value);
void set_shader_vec4(shader* shader, const char* name, float x, float y, float z, float w);
void set_shader_vec4f(shader* shader, const char* name, vec4 value);

// Uniform caching
GLint shader_get_uniform_location(shader* shader, const char* name);

// Debug: List all active uniforms and check if specific uniform exists
void debug_shader_uniforms(shader* shader);
b8 debug_check_uniform(shader* shader, const char* name);
