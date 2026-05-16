#pragma once

#include "defines.h"

typedef struct texture {
    u32 id;
    i32 width;
    i32 height;
    i32 channels;
} texture;

AGINA_API b8 load_texture_from_file(const char* path, texture* out_texture);
AGINA_API b8 use_texture(texture* texture, u32 slot);
AGINA_API void destroy_texture(texture* texture);

