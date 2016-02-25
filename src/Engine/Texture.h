#pragma once

#include "gl_core_4_4.h"

#include <assert.h>


/*
    Represents a texture owned by OpenGL.
*/
class Texture
{
public:
    enum class TextureType { RGB, RGBA };

    // Construct an invalid texture, call create later.
    Texture() : Texture(-1) {}

    // Takes ownership of the texture data, should be able to call destroy on any Texture object.
    Texture(GLuint textureId) : m_textureId(textureId) {}

    ~Texture() = default;

    bool isValid() { return m_textureId != (GLuint)-1; }
    GLuint getId() const { return m_textureId; }

    // Create and upload the OpenGL texture using the data provided.
    void create(const unsigned char* data, int imageWidth, int imageHeight, TextureType textureType);

    // Create a texture from a float buffer
    void create(const float* data, int imageWidth, int imageHeight);

    void destroy();

private:
    void create_impl(
        const void* data, // The image data buffer
        int imageWidth, int imageHeight, // The image dimensions in texels
        GLuint internalFormat,  // The format OpenGL should use to store the data
        GLuint format,  // The format the data is provided in
        GLuint dataType // The type of data stored in the image buffer
        );


    GLuint m_textureId;
};
