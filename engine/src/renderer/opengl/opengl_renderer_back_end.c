#include <glad/glad.h>
#include "opengl_renderer_back_end.h"

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

