#pragma once

#include "defines.h"
#include "model.h"
#include "shader.h"

void renderer_back_end_init();
void renderer_back_end_shutdown();

void renderer_back_end_clear_screen(float r, float g, float b, float a);
void renderer_back_end_on_resize(u32 width, u32 height); 

void debug_check(const char* msg);

