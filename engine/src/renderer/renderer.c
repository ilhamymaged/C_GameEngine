#include "renderer.h"

#include "opengl/opengl_renderer_front_end.h"
#include "core/logger.h"

static renderer_api renderer_api_in_use;

void renderer_init(renderer_api api, platform_state* plat_state, u32 width, u32 height) {
    renderer_api_in_use = api;
    switch (api) {
        case RENDERER_API_OPENGL:
            opengl_renderer_init(plat_state, width, height);
            break;
        default:
            AG_ERROR("Unsupported renderer API specified in renderer_init");
            break;
    }
}

void renderer_shutdown() {
    switch (renderer_api_in_use) {
        case RENDERER_API_OPENGL:
            opengl_renderer_shutdown();
            break;
        default:
            AG_ERROR("Unsupported renderer API specified in renderer_shutdown");
            break;
    }
}

void renderer_clear_screen(float r, float g, float b, float a) {
    switch (renderer_api_in_use) {
        case RENDERER_API_OPENGL:
            opengl_renderer_clear_screen(r, g, b, a);
            break;
        default:
            AG_ERROR("Unsupported renderer API specified in renderer_clear_screen");
            break;
    }
}

b8 renderer_on_resize(u16 code, void* sender, void* listener_inst, event_context data) {
    switch (renderer_api_in_use) {
        case RENDERER_API_OPENGL:
            return opengl_renderer_on_resize(code, sender, listener_inst, data);
        default:
            AG_ERROR("Unsupported renderer API specified in renderer_on_resize");
            return FALSE;
    }
}

void renderer_begin_frame(float r, float g, float b, float a) {
    switch (renderer_api_in_use) {
        case RENDERER_API_OPENGL:
            opengl_renderer_begin_frame(r, g, b, a);
            break;
        default:
            AG_ERROR("Unsupported renderer API specified in renderer_begin_frame");
            break;
    }
}

void renderer_end_frame() {
    switch (renderer_api_in_use) {
        case RENDERER_API_OPENGL:
            opengl_renderer_end_frame();
            break;
        default:
            AG_ERROR("Unsupported renderer API specified in renderer_end_frame");
            break;
    }
}

void renderer_begin_ui_frame() {
    switch (renderer_api_in_use) {
        case RENDERER_API_OPENGL:
            opengl_renderer_begin_ui_frame();
            break;
        default:
            AG_ERROR("Unsupported renderer API specified in renderer_end_frame");
            break;
    }
}

void renderer_end_ui_frame() {
    switch (renderer_api_in_use) {
        case RENDERER_API_OPENGL:
            opengl_renderer_end_ui_frame();
            break;
        default:
            AG_ERROR("Unsupported renderer API specified in renderer_end_frame");
            break;
    }
}

