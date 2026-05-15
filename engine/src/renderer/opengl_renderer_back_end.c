#include <glad/glad.h>
#include "opengl_renderer_back_end.h"
#include "model.h"

#include <core/logger.h>

void debug_check(const char* msg) {
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        AG_ERROR("%s OpenGL Error: %d", msg, err);
    }
}

void renderer_back_end_init() {
    
    if (!gladLoadGL()) {
        AG_FATAL("Failed to initialize OpenGL context!");
    }

    GLint major = 0, minor = 0;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);
    AG_INFO("OpenGL context initialized successfully Version: %d.%d", major, minor);

    glEnable(GL_DEPTH_TEST);
}
    
void renderer_back_end_shutdown() {
    AG_INFO("Shutting down OpenGL context.");
}

void renderer_back_end_clear_screen(float r, float g, float b, float a) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(r, g, b, a);
}

void renderer_back_end_on_resize(u32 width, u32 height) {
    AG_INFO("Resizing viewport to %ux%u", width, height);
    glViewport(0, 0, width, height);
}

void renderer_back_end_create_shader(shader* shader, const char* vertex_source, const char* fragment_source) {
    if (!create_shader(shader, vertex_source, fragment_source)) {
        AG_ERROR("Failed to create shader from sources: %s and %s", vertex_source, fragment_source);
    }
}

void renderer_back_end_use_shader(shader* shader) {
    use_shader(shader);
}

void renderer_back_end_destroy_shader(shader* shader) {
    destroy_shader(shader);
}

void renderer_back_end_set_uniform_mat4(shader* shader, const char* name, mat4 value) {
    set_shader_mat4(shader, name, value);
}

void renderer_back_end_set_uniform_vec3(shader* shader, const char* name, vec3 value) {
    set_shader_vec3(shader, name, value);
}

void renderer_back_end_set_uniform_float(shader* shader, const char* name, float value) {
    set_shader_float(shader, name, value);
}

void renderer_back_end_set_uniform_int(shader* shader, const char* name, int value) {
    set_shader_int(shader, name, value);
}

void renderer_back_end_set_uniform_vec4(shader* shader, const char* name, float x, float y, float z, float w) {
    set_shader_vec4(shader, name, x, y, z, w);
}

void renderer_back_end_set_uniform_vec4f(shader* shader, const char* name, vec4 value) {
    set_shader_vec4f(shader, name, value);
}

void renderer_back_end_debug_shader_uniforms(shader* shader) {
    debug_shader_uniforms(shader);
}

void renderer_back_end_init_model(model* m, const char* file_path) {
    init_model(m, file_path);
}

void renderer_back_end_draw_model(model* m) {
    draw_model(m);
}

void renderer_back_end_destroy_model(model* m) {
    destroy_model(m);
}

void renderer_back_end_debug_model_gpu_status(model* m, const char* model_name) {
    debug_model_gpu_status(m, model_name);
}

void renderer_back_end_init_mesh(mesh* m, float *vertices, u64 vertex_count, u32* indices, u64 index_count) {
    init_mesh(m, vertices, vertex_count, indices, index_count);
}

void renderer_back_end_draw_mesh(mesh* m) {
    draw_mesh(m);
}

void renderer_back_end_destroy_mesh(mesh* m) {
    destroy_mesh(m);
}



