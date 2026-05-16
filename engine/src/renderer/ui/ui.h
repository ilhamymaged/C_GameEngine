#pragma once

#include <defines.h>
#include "utility/math.h"
#include "renderer/opengl/shader.h"
#include "renderer/opengl/model.h"

typedef struct ui_context {
    shader shader;
    model quad;
    u32 width;
    u32 height;
} ui_context;

AGINA_API b8 ui_init(ui_context* ctx, u32 width, u32 height);
AGINA_API void ui_shutdown(ui_context* ctx);

AGINA_API void ui_begin(ui_context* ctx);

AGINA_API void ui_draw_rect(ui_context* ctx, float x, float y, float w, float h);