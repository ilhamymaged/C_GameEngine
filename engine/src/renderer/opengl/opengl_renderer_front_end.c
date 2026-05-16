#include "opengl_renderer_front_end.h"

#include "core/AGmemory.h"

#include <core/logger.h>

#include "model.h"

static platform_state* state;

typedef struct renderer_state {
    u32 width;
    u32 height;
}renderer_state;

struct renderer_state* r_state;

void opengl_renderer_init(platform_state* plat_state, u32 width, u32 height) {
    renderer_back_end_init();
    state = plat_state;
    r_state = AGallocate(sizeof(struct renderer_state), MEMORY_TAG_RENDERER);
    r_state->width = width;
    r_state->height = height;

    event_register(EVENT_CODE_RESIZED, state, opengl_renderer_on_resize); 
}

void opengl_renderer_shutdown() {
    renderer_back_end_shutdown();
    event_unregister(EVENT_CODE_RESIZED, state, opengl_renderer_on_resize);
    AGfree(r_state, sizeof(struct renderer_state), MEMORY_TAG_RENDERER);
}

void opengl_renderer_clear_screen(float r, float g, float b, float a) {
    renderer_back_end_clear_screen(r, g, b, a);
}

void opengl_renderer_begin_frame(float r, float g, float b, float a) {
    // For now, nothing to do here. In the future, we may want to add some per-frame setup code here.
    opengl_renderer_clear_screen(r, g, b, a);
}

b8 opengl_renderer_on_resize(u16 code, void* sender, void* listener_inst, event_context data) {
    (void)listener_inst;
    u32 width = data.data.u32[0];
    u32 height = data.data.u32[1];
    r_state->width = width;
    r_state->height = height;
    renderer_back_end_on_resize(width, height);
    return TRUE;
}

void opengl_renderer_end_frame() {
    platform_swap_buffers(state);
}

void opengl_renderer_begin_ui_frame() {
    opengl_renderer_back_end_begin_ui_frame();
}

void opengl_renderer_end_ui_frame() {
    opengl_renderer_back_end_end_ui_frame();
}