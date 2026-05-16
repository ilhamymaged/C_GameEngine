#include "model.h"

#include <glad/glad.h>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <core/AGmemory.h>
#include <string.h>
#include <core/logger.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

void init_quad(model* m) {

    quad_desc* desc = (quad_desc*)m->add_data;
    float vertices[] = {
        // pos               // color        // uv
        -1.0f, -1.0f, 0.0f,   desc->color.x, desc->color.y, desc->color.z,        0.0f, 0.0f,
         1.0f, -1.0f, 0.0f,   desc->color.x, desc->color.y, desc->color.z,        1.0f, 0.0f,
         1.0f,  1.0f, 0.0f,   desc->color.x, desc->color.y, desc->color.z,        1.0f, 1.0f,
        -1.0f,  1.0f, 0.0f,   desc->color.x, desc->color.y, desc->color.z,        0.0f, 1.0f
    };

    u32 indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    m->mesh_count = 1;
    m->meshes = (mesh*)AGallocate(sizeof(mesh), MEMORY_TAG_MODEL);

    init_mesh(&m->meshes[0],
              vertices,
              4,
              indices,
              6);
}

void init_sphere(model* m) {
    sphere_desc* desc = (sphere_desc*)m->add_data;

    // Default fallbacks if no descriptor is provided
    float radius =  desc->radius == 0 ?  0.5f : desc->radius;
    int sectors =  desc->sectors == 0 ?  36 : desc->sectors; 
    int stacks =    desc->stacks == 0 ?  36 : desc->stacks;

    // Calculate total counts
    u64 vertex_count = (stacks + 1) * (sectors + 1);
    u64 index_count = stacks * sectors * 6;

    // Your engine utilizes 8 floats per vertex (3 pos, 3 norm, 2 uv)
    float* vertices = (float*)AGallocate(vertex_count * 8 * sizeof(float), MEMORY_TAG_MODEL);
    u32* indices = (u32*)AGallocate(index_count * sizeof(u32), MEMORY_TAG_MODEL);

    int v = 0;
    float x, y, z, xy;                              // vertex position
    float lengthInv = 1.0f / radius;                // Inverse radius for quick normalization
    float s, t;                                     // vertex texCoords

    float sectorStep = 2 * M_PI / sectors;
    float stackStep = M_PI / stacks;
    float sectorAngle, stackAngle;

    // 1. Generate vertices, normals, and UVs
    for (int i = 0; i <= stacks; ++i) {
        stackAngle = M_PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
        xy = radius * cosf(stackAngle);             // r * cos(u)
        z = radius * sinf(stackAngle);              // r * sin(u)

        for (int j = 0; j <= sectors; ++j) {
            sectorAngle = j * sectorStep;           // starting from 0 to 2pi

            // Position
            x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
            y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
            
            // In OpenGL, usually Y is up, so we swap coordinates to make it stand upright
            vertices[v++] = x;
            vertices[v++] = z; // Y position
            vertices[v++] = y; // Z position

            // Normalized Normals
            vertices[v++] = x * lengthInv;
            vertices[v++] = z * lengthInv;
            vertices[v++] = y * lengthInv;

            // Vertex TexCoords (UVs)
            s = (float)j / sectors;
            t = (float)i / stacks;
            vertices[v++] = s;
            vertices[v++] = t;
        }
    }

    // 2. Generate indices (stitching the stacks and sectors into triangles)
    int k1, k2;
    int idx = 0;
    for (int i = 0; i < stacks; ++i) {
        k1 = i * (sectors + 1);     // beginning of current stack
        k2 = k1 + sectors + 1;      // beginning of next stack

        for (int j = 0; j < sectors; ++j, ++k1, ++k2) {
            // 2 triangles per sector grid quad
            // k1---k1+1
            // |  /  |
            // k2---k2+1
            
            // Triangle 1: k1 -> k2 -> k1+1
            indices[idx++] = k1;
            indices[idx++] = k2;
            indices[idx++] = k1 + 1;

            // Triangle 2: k1+1 -> k2 -> k2+1
            indices[idx++] = k1 + 1;
            indices[idx++] = k2;
            indices[idx++] = k2 + 1;
        }
    }

    m->mesh_count = 1;
    m->meshes = (mesh*)AGallocate(sizeof(mesh), MEMORY_TAG_MODEL);

    mesh* sphere_mesh = &m->meshes[0];
    init_mesh(sphere_mesh, vertices, vertex_count, indices, index_count);

    AGfree(vertices, vertex_count * 8 * sizeof(float), MEMORY_TAG_MODEL);
    AGfree(indices, index_count * sizeof(u32), MEMORY_TAG_MODEL);
}

#define DEFAULT_GRID_SIZE 100
#define DEFAULT_GRID_DIVISIONS 100

void init_grid(model* m) {
    grid_desc* desc = (grid_desc*)m->add_data;

    int size = desc->size == 0 ? DEFAULT_GRID_SIZE : desc->size;
    int divisions = desc->divisions == 0 ? DEFAULT_GRID_DIVISIONS : desc->divisions;

    int line_count = divisions + 1;
    float step = (float)size / divisions;
    float half = (float)size * 0.5f;

    int total_lines = line_count * 2;

    // 2 vertices per line
    int vertex_count = total_lines * 2;

    // each line = 2 indices
    int index_count = total_lines * 2;

    float* vertices = (float*)AGallocate(vertex_count * 8 * sizeof(float), MEMORY_TAG_MODEL);
    u32* indices = (u32*)AGallocate(index_count * sizeof(u32), MEMORY_TAG_MODEL);

    int v = 0;
    int i = 0;
    u32 base_vertex = 0;

    for (int j = 0; j < line_count; j++) {
        float offset = -half + j * step;

        // ---- X axis line ----
        vertices[v++] = -half; vertices[v++] = 0; vertices[v++] = offset;
        vertices[v++] = 0;     vertices[v++] = 1; vertices[v++] = 0;
        vertices[v++] = 0;     vertices[v++] = 0;

        vertices[v++] = half;  vertices[v++] = 0; vertices[v++] = offset;
        vertices[v++] = 0;     vertices[v++] = 1; vertices[v++] = 0;
        vertices[v++] = 0;     vertices[v++] = 0;

        indices[i++] = base_vertex + 0;
        indices[i++] = base_vertex + 1;
        base_vertex += 2;

        // ---- Z axis line ----
        vertices[v++] = offset; vertices[v++] = 0; vertices[v++] = -half;
        vertices[v++] = 0;      vertices[v++] = 1; vertices[v++] = 0;
        vertices[v++] = 0;      vertices[v++] = 0;

        vertices[v++] = offset; vertices[v++] = 0; vertices[v++] = half;
        vertices[v++] = 0;      vertices[v++] = 1; vertices[v++] = 0;
        vertices[v++] = 0;      vertices[v++] = 0;

        indices[i++] = base_vertex + 0;
        indices[i++] = base_vertex + 1;
        base_vertex += 2;
    }

    m->mesh_count = 1;
    m->meshes = (mesh*)AGallocate(sizeof(mesh), MEMORY_TAG_MODEL);

    mesh* g = &m->meshes[0];
    init_mesh(g, vertices, vertex_count, indices, index_count);

    AGfree(vertices, vertex_count * 8 * sizeof(float), MEMORY_TAG_MODEL);
    AGfree(indices, index_count * sizeof(u32), MEMORY_TAG_MODEL);
}

void init_cube(model* c) {
    float vertices[] = {
    // positions          // normals        // uv
    -0.5f, -0.5f, -0.5f,   0, 0, -1,       0, 0, // 0 back bottom left
     0.5f, -0.5f, -0.5f,   0, 0, -1,       1, 0, // 1 back bottom right
     0.5f,  0.5f, -0.5f,   0, 0, -1,       1, 1, // 2 back top right
    -0.5f,  0.5f, -0.5f,   0, 0, -1,       0, 1, // 3 back top left

    -0.5f, -0.5f,  0.5f,   0, 0,  1,       0, 0, // 4 front bottom left
     0.5f, -0.5f,  0.5f,   0, 0,  1,       1, 0, // 5 front bottom right
     0.5f,  0.5f,  0.5f,   0, 0,  1,       1, 1, // 6 front top right
    -0.5f,  0.5f,  0.5f,   0, 0,  1,       0, 1  // 7 front top left
    };

    u32 indices[] = {
        // back face
        0, 1, 2,
        2, 3, 0,

        // front face
        4, 5, 6,
        6, 7, 4,

        // left face
        4, 0, 3,
        3, 7, 4,

        // right face
        1, 5, 6,
        6, 2, 1,

        // bottom face
        4, 5, 1,
        1, 0, 4,

        // top face
        3, 2, 6,
        6, 7, 3
    };

    u64 vertex_count = 8; // 8 total vertices
    u64 index_count = sizeof(indices) / sizeof(u32); // 36

    init_mesh(&c->meshes[0], vertices, vertex_count, indices, index_count);
}

void destroy_mesh(mesh* m) {
    if (m->vao != 0) {
        glDeleteVertexArrays(1, &m->vao);
        m->vao = 0;
    }
    if (m->vbo != 0) {
        glDeleteBuffers(1, &m->vbo);
        m->vbo = 0;
    }
    if (m->ebo != 0) {
        glDeleteBuffers(1, &m->ebo);
        m->ebo = 0;
    }
}

void init_mesh(mesh* m, float *vertices, u64 vertex_count, u32* indices, u64 index_count) {
    m->vertex_count = vertex_count;
    m->index_count = index_count;

    glGenVertexArrays(1, &m->vao);
    glGenBuffers(1, &m->vbo);
    glGenBuffers(1, &m->ebo);

    glBindVertexArray(m->vao);
    glBindBuffer(GL_ARRAY_BUFFER, m->vbo);
    glBufferData(GL_ARRAY_BUFFER, vertex_count * 8 * sizeof(float), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_count * sizeof(u32), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void draw_mesh(mesh* m) {
    glBindVertexArray(m->vao);
    glDrawElements(GL_TRIANGLES, (GLsizei)m->index_count, GL_UNSIGNED_INT, (void*)0);
    glBindVertexArray(0);
}

void draw_mesh_lines(mesh* m) {
    glBindVertexArray(m->vao);
    glDrawElements(GL_LINES, (GLsizei)m->index_count, GL_UNSIGNED_INT, (void*)0);
    glBindVertexArray(0);
}

void init_model(model* m, const char* file_path, model_type type, void* add_data) {

    m->type = type;
    m->add_data = add_data;
    if (type == CUBE && file_path == NULL) {
        m->mesh_count = 1;
        m->meshes = (mesh*)AGallocate(sizeof(mesh), MEMORY_TAG_MODEL);
        if (m->meshes == NULL) {
            AG_ERROR("Failed to allocate memory for cube model");
            return;
        }
        init_cube(m);
        return;
    }

    else if (type == QUAD && file_path == NULL) {
        init_quad(m);
        m->add_data = NULL;
        return;
    }

    else if (type == GRID && file_path == NULL) {
        init_grid(m);
        m->add_data = NULL;
        return;
    }

    else if (type == SPHERE && file_path == NULL) {
        init_sphere(m);
        m->add_data = NULL;
        return;
    }

    else if (type != MODEL_FILE) {
        AG_ERROR("Unsupported model type specified for init_model");
        return;
    }

    m->meshes = NULL;
    m->mesh_count = 0;

    const struct aiScene* scene = aiImportFile(file_path,
        aiProcess_Triangulate |
        aiProcess_GenSmoothNormals |
        aiProcess_FlipUVs |
        aiProcess_JoinIdenticalVertices);

    if (scene == NULL) {
        AG_ERROR("Failed to load model from path: %s. Assimp error: %s", file_path, aiGetErrorString());
        return;
    }

    m->mesh_count = scene->mNumMeshes;
    if (m->mesh_count == 0) {
        aiReleaseImport(scene);
        return;
    }

    m->meshes = (mesh*)AGallocate(sizeof(mesh) * m->mesh_count, MEMORY_TAG_MODEL);
    if (m->meshes == NULL) {
        aiReleaseImport(scene);
        return;
    }
    AGzero_memory(m->meshes, sizeof(mesh) * m->mesh_count);

    for (u64 mesh_index = 0; mesh_index < m->mesh_count; ++mesh_index) {
        struct aiMesh* ai_mesh = scene->mMeshes[mesh_index];
        u64 vertex_count = ai_mesh->mNumVertices;
        u64 index_count = ai_mesh->mNumFaces * 3;

        float* vertices = (float*)AGallocate(vertex_count * 8 * sizeof(float), MEMORY_TAG_MODEL);
        u32* indices = (u32*)AGallocate(index_count * sizeof(u32), MEMORY_TAG_MODEL);

        if (vertices == NULL || indices == NULL) {
            AGfree(vertices, vertex_count * 8 * sizeof(float), MEMORY_TAG_MODEL);
            AGfree(indices, index_count * sizeof(u32), MEMORY_TAG_MODEL);
            continue;
        }

        for (u64 v = 0; v < vertex_count; ++v) {
            u64 base = v * 8;
            vertices[base + 0] = ai_mesh->mVertices[v].x;
            vertices[base + 1] = ai_mesh->mVertices[v].y;
            vertices[base + 2] = ai_mesh->mVertices[v].z;

            vertices[base + 3] = ai_mesh->mNormals ? ai_mesh->mNormals[v].x : 0.0f;
            vertices[base + 4] = ai_mesh->mNormals ? ai_mesh->mNormals[v].y : 0.0f;
            vertices[base + 5] = ai_mesh->mNormals ? ai_mesh->mNormals[v].z : 0.0f;

            if (ai_mesh->mTextureCoords[0]) {
                vertices[base + 6] = ai_mesh->mTextureCoords[0][v].x;
                vertices[base + 7] = ai_mesh->mTextureCoords[0][v].y;
            } else {
                vertices[base + 6] = 0.0f;
                vertices[base + 7] = 0.0f;
            }
        }

        for (u64 f = 0; f < ai_mesh->mNumFaces; ++f) {
            const struct aiFace* face = &ai_mesh->mFaces[f];
            u64 base = f * 3;
            indices[base + 0] = face->mIndices[0];
            indices[base + 1] = face->mIndices[1];
            indices[base + 2] = face->mIndices[2];
        }

        init_mesh(&m->meshes[mesh_index], vertices, vertex_count, indices, index_count);
        AGfree(vertices, vertex_count * 8 * sizeof(float), MEMORY_TAG_MODEL);
        AGfree(indices, index_count * sizeof(u32), MEMORY_TAG_MODEL);
    }

    aiReleaseImport(scene);
}

void draw_model(model* m) {
    for (u64 i = 0; i < m->mesh_count; ++i) {
        draw_mesh(&m->meshes[i]);
    }
}

void draw_model_lines(model* m) {
    for (u64 i = 0; i < m->mesh_count; ++i) {
        draw_mesh_lines(&m->meshes[i]);
    }
}

void destroy_model(model* m) {
    if (m->meshes == NULL || m == NULL) {
        return;
    }

    for (u64 i = 0; i < m->mesh_count; ++i) {
        destroy_mesh(&m->meshes[i]);
    }

    AGfree(m->meshes, sizeof(mesh) * m->mesh_count, MEMORY_TAG_MODEL);
    m->meshes = NULL;
    m->mesh_count = 0;
}

void debug_model_gpu_status(model* m, const char* model_name) {
    if (!m || !m->meshes) {
        AG_WARN("[GPU_DEBUG] Model '%s': NOT LOADED - no mesh data", model_name);
        return;
    }

    AG_INFO("[GPU_DEBUG] Model '%s': %llu mesh(es)", model_name, m->mesh_count);

    u64 total_vram = 0;

    for (u64 i = 0; i < m->mesh_count; ++i) {
        mesh* msh = &m->meshes[i];
        
        if (msh->vao == 0 || msh->vbo == 0 || msh->ebo == 0) {
            AG_WARN("[GPU_DEBUG]   Mesh %llu: INVALID - VAO=%u VBO=%u EBO=%u", i, msh->vao, msh->vbo, msh->ebo);
            continue;
        }

        // Query buffer sizes from GPU
        GLint vbo_size = 0, ebo_size = 0;
        glBindBuffer(GL_ARRAY_BUFFER, msh->vbo);
        glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &vbo_size);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, msh->ebo);
        glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &ebo_size);

        u64 mesh_vram = vbo_size + ebo_size;
        total_vram += mesh_vram;

        AG_INFO("[GPU_DEBUG]   Mesh %llu: CACHED - VAO=%u VBO=%u(%u bytes) EBO=%u(%u bytes) Vertices=%llu Indices=%llu",
                i, msh->vao, msh->vbo, vbo_size, msh->ebo, ebo_size, msh->vertex_count, msh->index_count);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    AG_INFO("[GPU_DEBUG] Model '%s': Total VRAM used: %llu bytes", model_name, total_vram);
}

void init_model_instance(model_instance* instance, model* m, material* mat) {
    instance->model = m;
    instance->mat = mat;
}

void draw_model_instance(model_instance* instance) {
    if (instance->mat) {
        use_material(instance->mat);
    }
    draw_model(instance->model);
}   