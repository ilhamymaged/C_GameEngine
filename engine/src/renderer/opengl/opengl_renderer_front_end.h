#pragma once

#include "defines.h"

#include "platform/platform.h"

#include "core/event.h"

#include "opengl_renderer_back_end.h"

void opengl_renderer_init(platform_state* plat_state, u32 width, u32 height);
void opengl_renderer_shutdown();

void opengl_renderer_clear_screen(float r, float g, float b, float a);
b8 opengl_renderer_on_resize(u16 code, void* sender, void* listener_inst, event_context data);

void opengl_renderer_begin_frame();
void opengl_renderer_end_frame();
