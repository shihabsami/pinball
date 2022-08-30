#ifndef LOADER_H
#define LOADER_H

#include <glad/glad.h>

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION

#include <stb/stb_image.h>

static unsigned int load_texture(const char* file, const std::string& directory, bool gamma_correction) {
    std::string filename = std::string(file);
    filename = directory + '/' + filename;

    unsigned int texture_id = 0;
    glGenTextures(1, &texture_id);

    int width, height, channels;
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &channels, 0);
    if (data) {
        GLenum internal_format;
        GLenum data_format;
        if (channels == 1) {
            internal_format = data_format = GL_RED;
        } else if (channels == 3) {
            internal_format = gamma_correction ? GL_SRGB : GL_RGB;
            data_format = GL_RGB;
        } else if (channels == 4) {
            internal_format = gamma_correction ? GL_SRGB_ALPHA : GL_RGBA;
            data_format = GL_RGBA;
        }

        glBindTexture(GL_TEXTURE_2D, texture_id);
        glTexImage2D(GL_TEXTURE_2D, 0, (GLint) internal_format, width, height, 0, data_format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } else {
        glDeleteTextures(1, &texture_id);
        fprintf(stderr, "ERROR: Error loading texture: %s\n", file);
        stbi_image_free(data);
        return 0;
    }

    stbi_image_free(data);
    return texture_id;
}

static unsigned int load_cubemap(const std::vector<const char*>& files) {
    unsigned int texture_id = 0;
    glGenTextures(1, &texture_id);

    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);
    int width, height, channels;
    for (unsigned int i = 0; i < files.size(); i++) {
        unsigned char* data = stbi_load(files[i], &width, &height, &channels, 0);
        if (data) {
            GLenum format;
            if (channels == 1)
                format = GL_RED;
            else if (channels == 3)
                format = GL_RGB;
            else if (channels == 4)
                format = GL_RGBA;

            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, (GLint) format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        } else {
            glDeleteTextures(1, &texture_id);
            fprintf(stderr, "ERROR: Error loading cubemap: %s\n", files[i]);
            stbi_image_free(data);
            return 0;
        }

        stbi_image_free(data);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return texture_id;
}

#endif // LOADER_H
