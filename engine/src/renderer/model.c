#include "model.h"

#include <glad/glad.h>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <core/AGmemory.h>
#include <string.h>
#include <core/logger.h>

static void destroy_mesh(mesh* m) {
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

void init_model(model* m, const char* file_path) {
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
    memset(m->meshes, 0, sizeof(mesh) * m->mesh_count);

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

void destroy_model(model* m) {
    if (m->meshes == NULL) {
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

