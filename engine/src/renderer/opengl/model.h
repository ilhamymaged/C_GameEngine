#pragma once

#include "defines.h"

#include "utility/math.h"

typedef enum model_type {
    TRIANGLE,
    QUAD,
    CUBE,
    MODEL_FILE,
    GRID,
    SPHERE
} model_type;

typedef struct quad_desc {
    vec3 color;
} quad_desc;

typedef struct grid_desc {
    int size;       // e.g. 10
    int divisions;  // e.g. 10
} grid_desc;

typedef struct sphere_desc {
    float radius;
    int sectors;   // Number of longitude slices (slices around the Z-axis)
    int stacks;    // Number of latitude stacks (rings from top to bottom)
} sphere_desc;

typedef struct mesh {
    u32 vao;
    u32 vbo;
    u32 ebo;
    u64 vertex_count;
    u64 index_count;
} mesh;

typedef struct model {
    model_type type;
    mesh* meshes;
    u64 mesh_count;
    void* add_data;
} model;

void init_mesh(mesh* m, float *vertices, u64 vertex_count, u32* indices, u64 index_count);    
void draw_mesh(mesh* m);
void draw_mesh_lines(mesh* m);
void destroy_mesh(mesh* m);

// Note: Using Assimp
AGINA_API void init_model(model* m, const char* file_path, model_type type, void* add_data);
AGINA_API void draw_model(model* m);
AGINA_API void draw_model_lines(model* m);
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