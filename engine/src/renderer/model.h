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

void init_mesh(mesh* m, float *vertices, u64 vertex_count, u32* indices, u64 index_count);
void draw_mesh(mesh* m);
void destroy_mesh(mesh* m);

// Note: Using Assimp
void init_model(model* m, const char* file_path);
void draw_model(model* m);
void destroy_model(model* m);

// Debug: Check if model is cached in GPU
void debug_model_gpu_status(model* m, const char* model_name);