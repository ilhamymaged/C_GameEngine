#include "texture.h"
#include <stb_image/stb_image.h>
#include <glad/glad.h>
#include <core/logger.h>
#include <core/AGmemory.h>
#include <string.h>

b8 load_texture_from_file(const char* path, texture* out_texture) {

    const char* filename = strrchr(path, '/');

    #ifdef _WIN32
    // Support Windows paths too
    const char* backslash = strrchr(path, '\\');
    if (!filename || (backslash && backslash > filename)) {
        filename = backslash;
    }
    #endif

    filename = filename ? filename + 1 : path;

    size_t length = strlen(filename);

    if (length >= sizeof(out_texture->name)) {
        length = sizeof(out_texture->name) - 1;
    }

    AGcopy_memory(out_texture->name, filename, length);
    out_texture->name[length] = '\0';

    glGenTextures(1, &out_texture->id);
    glBindTexture(GL_TEXTURE_2D, out_texture->id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // stbi_set_flip_vertically_on_load(1);
    unsigned char* data = stbi_load(path, &out_texture->width, &out_texture->height, &out_texture->channels, 0);
    if (data) {

        GLenum format = GL_RGB;
        if (out_texture->channels == 1) {
            format = GL_RED;
        } else if (out_texture->channels == 3) {
            format = GL_RGB;
        } else if (out_texture->channels == 4) {
            format = GL_RGBA;
        }

        glTexImage2D(GL_TEXTURE_2D, 0, format, out_texture->width, out_texture->height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(data);
        AG_INFO(
            "Loaded texture with NAME: %s (ID: %u, Size: %dx%d, Channels: %d)", 
            out_texture->name, out_texture->id,
            out_texture->width, 
            out_texture->height, 
            out_texture->channels);
        return TRUE;

    } else {
        AG_ERROR("Failed to load texture from path: %s", path);
        return FALSE;
    }
}

b8 use_texture(texture* texture, u32 slot) {
    if (!texture) {
        AG_ERROR("Invalid texture provided.");
        return FALSE;
    }
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, texture->id);
    return TRUE;
}

void destroy_texture(texture* texture) {
    if (texture) {
        glDeleteTextures(1, &texture->id);
        texture->id = 0;
        texture->width = 0;
        texture->height = 0;
        texture->channels = 0;
    }
}