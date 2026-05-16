#pragma once

#include "defines.h"

typedef struct mesh {
    u32 vao;
    u32 vbo;
    u32 ebo;
    u64 vertex_count;
    u64 index_count;
} mesh;

typedef struct model {
    mesh* meshes;
    u64 mesh_count;
} model;

AGINA_API void init_mesh(mesh* m, float *vertices, u64 vertex_count, u32* indices, u64 index_count);
AGINA_API void draw_mesh(mesh* m);
AGINA_API void destroy_mesh(mesh* m);

// Note: Using Assimp
AGINA_API void init_model(model* m, const char* file_path);
AGINA_API void draw_model(model* m);
AGINA_API void destroy_model(model* m);

// Debug: Check if model is cached in GPU
AGINA_API void debug_model_gpu_status(model* m, const char* model_name);

#include "material.h"

typedef struct model_instance {
    model* model;
    material* mat;
} model_instance;

AGINA_API void draw_model_instance(model_instance* instance);
AGINA_API void init_model_instance(model_instance* instance, model* m, material* mat);
/* NOTE:
 No Destroy function since model_instance doesn't own the shader or texture, 
 it just references them. The user is responsible for managing the lifetime of the shader and texture.
*/ 