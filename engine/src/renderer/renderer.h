#pragma once

#include "defines.h"

#include "platform/platform.h"

#include "core/event.h"

typedef enum renderer_api {
    RENDERER_API_OPENGL,
    RENDERER_API_VULKAN,
    RENDERER_API_DIRECTX12
} renderer_api;

void renderer_init(renderer_api api, platform_state* plat_state, u32 width, u32 height);
void renderer_shutdown();

void renderer_clear_screen(float r, float g, float b, float a);
b8 renderer_on_resize(u16 code, void* sender, void* listener_inst, event_context data);

void renderer_begin_frame();
void renderer_end_frame();

