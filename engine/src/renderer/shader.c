#include "shader.h"

#include <glad/glad.h>
#include <utility/parser.h>
#include <core/AGmemory.h>
#include <core/logger.h>
#include <string.h>

#define SHADER_CLEANUP() \
    glDeleteShader(vertex_shader); \
    glDeleteShader(fragment_shader); \
    AGfree(vertex_code, vertex_length, MEMORY_TAG_SHADER); \
    AGfree(fragment_code, fragment_length, MEMORY_TAG_SHADER);

b8 create_shader(shader* shader, const char* vertex_source, const char* fragment_source) {
    u64 vertex_length, fragment_length;
    char* vertex_code = read_file(vertex_source, &vertex_length);
    char* fragment_code = read_file(fragment_source, &fragment_length);

    if (!vertex_code || !fragment_code) {
        AG_ERROR("Failed to read shader source files.");
        return FALSE;
    }

    u32 vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, (const char**)&vertex_code, NULL);
    glCompileShader(vertex_shader);

    // Note: Check for compilation errors.
    int error;
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &error);
    if (!error) {
        char info_log[512];
        glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
        AG_ERROR("Vertex shader compilation failed: %s", info_log);

        glDeleteShader(vertex_shader);

        AGfree(vertex_code, vertex_length, MEMORY_TAG_SHADER);
        AGfree(fragment_code, fragment_length, MEMORY_TAG_SHADER);
        return FALSE;
    }

    u32 fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, (const char**)&fragment_code, NULL);
    glCompileShader(fragment_shader);

    // Note: Check for compilation errors.
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &error);
    if (!error) {
        char info_log[512];
        glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
        AG_ERROR("Fragment shader compilation failed: %s", info_log);

        SHADER_CLEANUP();
        return FALSE;
    }

    shader->id = glCreateProgram();
    if (!shader->id) {
        AG_ERROR("Failed to create shader program");
        SHADER_CLEANUP();
        return FALSE;
    }

    glAttachShader(shader->id, vertex_shader);
    glAttachShader(shader->id, fragment_shader);
    glLinkProgram(shader->id);

    // Note: Check for linking errors.
    glGetProgramiv(shader->id, GL_LINK_STATUS, &error);
    if (!error) {
        char info_log[512];
        glGetProgramInfoLog(shader->id, 512, NULL, info_log);
        AG_ERROR("Shader program linking failed: %s", info_log);

        SHADER_CLEANUP();
        return FALSE;
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    AGfree(vertex_code, vertex_length, MEMORY_TAG_SHADER);
    AGfree(fragment_code, fragment_length, MEMORY_TAG_SHADER);  

    AG_INFO("Shader program created successfully with ID: %u", shader->id);

    // Initialize uniform cache for this shader
    shader->uniform_cache = (uniform_cache_entry*)AGallocate(
        sizeof(uniform_cache_entry) * MAX_SHADER_UNIFORMS, MEMORY_TAG_SHADER_UNIFORM);
    AGzero_memory(shader->uniform_cache, sizeof(uniform_cache_entry) * MAX_SHADER_UNIFORMS);
    shader->uniform_cache_count = 0;

    return TRUE;
}

b8 use_shader(shader* shader) {
    if (!shader) {
        AG_ERROR("Attempted to use a NULL shader.");
        return FALSE;
    }
    glUseProgram(shader->id);
    return TRUE;
}

b8 destroy_shader(shader* shader) {
    if (!shader) {
        AG_ERROR("Attempted to destroy a NULL shader.");
        return FALSE;
    }
    glDeleteProgram(shader->id);
    
    if (shader->uniform_cache) {
        AGfree(shader->uniform_cache, sizeof(uniform_cache_entry) * MAX_SHADER_UNIFORMS, MEMORY_TAG_SHADER_UNIFORM);
        shader->uniform_cache = NULL;
        shader->uniform_cache_count = 0;
    }
    
    return TRUE;
}

GLint shader_get_uniform_location(shader* shader, const char* name) {
    if (!shader || !name) {
        return -1;
    }

    // Check if already cached
    for (u32 i = 0; i < shader->uniform_cache_count; ++i) {
        if (strcmp(shader->uniform_cache[i].name, name) == 0) {
            return shader->uniform_cache[i].location;
        }
    }

    // Not in cache, look it up
    GLint location = glGetUniformLocation(shader->id, name);

    // Cache it if there's space and it was found
    if (location >= 0 && shader->uniform_cache_count < MAX_SHADER_UNIFORMS) {
        uniform_cache_entry* entry = &shader->uniform_cache[shader->uniform_cache_count];
        strncpy(entry->name, name, sizeof(entry->name) - 1);
        entry->name[sizeof(entry->name) - 1] = '\0';
        entry->location = location;
        shader->uniform_cache_count++;
    }

    return location;
}

void set_shader_mat4(shader* shader, const char* name, mat4 value) {
    GLint location = shader_get_uniform_location(shader, name);
    if (location >= 0) {
        glUniformMatrix4fv(location, 1, GL_FALSE, value.elements);
    }
}

void set_shader_vec3(shader* shader, const char* name, vec3 value) {
    GLint location = shader_get_uniform_location(shader, name);
    if (location >= 0) {
        glUniform3f(location, value.x, value.y, value.z);
    }
}

void set_shader_float(shader* shader, const char* name, float value) {
    GLint location = shader_get_uniform_location(shader, name);
    if (location >= 0) {
        glUniform1f(location, value);
    }
}

void set_shader_int(shader* shader, const char* name, int value) {
    GLint location = shader_get_uniform_location(shader, name);
    if (location >= 0) {
        glUniform1i(location, value);
    }
}

void set_shader_vec4(shader* shader, const char* name, float x, float y, float z, float w) {
    GLint location = shader_get_uniform_location(shader, name);
    if (location >= 0) {
        glUniform4f(location, x, y, z, w);
    }
}

void set_shader_vec4f(shader* shader, const char* name, vec4 value) {
    GLint location = shader_get_uniform_location(shader, name);
    if (location >= 0) {
        glUniform4f(location, value.x, value.y, value.z, value.w);
    }
}

void debug_shader_uniforms(shader* shader) {
    if (!shader) {
        AG_ERROR("[SHADER_DEBUG] Attempted to debug NULL shader.");
        return;
    }

    GLint num_uniforms = 0;
    glGetProgramiv(shader->id, GL_ACTIVE_UNIFORMS, &num_uniforms);
    AG_INFO("[SHADER_DEBUG] Shader %u has %d active uniforms:", shader->id, num_uniforms);

    for (GLint i = 0; i < num_uniforms; ++i) {
        GLchar name[256];
        GLint size;
        GLenum type;
        glGetActiveUniform(shader->id, (GLuint)i, sizeof(name), NULL, &size, &type, name);

        const char* type_str = "UNKNOWN";
        switch (type) {
            case GL_FLOAT:      type_str = "FLOAT"; break;
            case GL_INT:        type_str = "INT"; break;
            case GL_FLOAT_VEC2: type_str = "VEC2"; break;
            case GL_FLOAT_VEC3: type_str = "VEC3"; break;
            case GL_FLOAT_VEC4: type_str = "VEC4"; break;
            case GL_FLOAT_MAT4: type_str = "MAT4"; break;
            case GL_FLOAT_MAT3: type_str = "MAT3"; break;
            case GL_SAMPLER_2D: type_str = "SAMPLER_2D"; break;
        }

        GLint location = glGetUniformLocation(shader->id, name);
        AG_INFO("  [%d] %s (type=%s, location=%d, size=%d)", i, name, type_str, location, size);
    }
}

b8 debug_check_uniform(shader* shader, const char* name) {
    if (!shader || !name) {
        AG_ERROR("[SHADER_DEBUG] Invalid shader or name.");
        return FALSE;
    }

    GLint location = glGetUniformLocation(shader->id, name);
    if (location >= 0) {
        AG_INFO("[SHADER_DEBUG] Uniform '%s' FOUND at location %d in shader %u", name, location, shader->id);
        return TRUE;
    } else {
        AG_WARN("[SHADER_DEBUG] Uniform '%s' NOT FOUND in shader %u (location = -1)", name, shader->id);
        return FALSE;
    }
}