#pragma once

#include "defines.h"
#include "texture.h"
#include "shader.h"
#include "utility/math.h"

typedef struct material {
    texture* albedo;

    shader* shader;

    vec3 color;

    b8 use_albedo_map;
} material;

AGINA_API void init_material(material* mat, shader* shader, texture* albedo, vec3 color, b8 use_albedo_map);
AGINA_API void use_material(material* mat);

/* NOTE:
 No Destroy function since material doesn't own the shader or texture, 
 it just references them. The user is responsible for managing the lifetime of the shader and texture.
*/ 

