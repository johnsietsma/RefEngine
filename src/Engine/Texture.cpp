#include "Texture.h"


void Texture::create(const unsigned char* data, int imageWidth, int imageHeight, TextureType textureType)
{
    assert(m_textureId == (GLuint)-1);

    GLuint internalFormat = -1; // How OpenGL will store it
    GLuint format = -1; // How the data given is arranged

    switch (textureType) {
    case TextureType::RGB: 
        internalFormat = GL_RGB;
        format = GL_RGB;
        break;
    case TextureType::RGBA:
        internalFormat = GL_RGBA;
        format = GL_RGBA;
        break;
    default:
        assert(false && "Unknown texture type");
        return;
    }

    create_impl(data, imageWidth, imageHeight, internalFormat, format, GL_UNSIGNED_BYTE);
}

void Texture::create(const float* data, int imageWidth, int imageHeight) 
{
    create_impl(
        data, 
        imageWidth, imageHeight, 
        GL_R32F,  // We're giving OpenGL a 32bit float
        GL_RED,   // OpenGL should store it in the red channel
        GL_FLOAT  // The data we're providing is floats
        );
}

void Texture::create_impl(const void* data, int imageWidth, int imageHeight, GLuint internalFormat, GLuint format, GLuint dataType)
{
    assert(m_textureId == (GLuint)-1);

    glGenTextures(1, &m_textureId);
    glBindTexture(GL_TEXTURE_2D, m_textureId);

    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, imageWidth, imageHeight, 0, format, dataType, data);

    // Use a linear filter.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // Clamp rather then wrap
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

}

void Texture::destroy()
{
    if (m_textureId != (GLuint)-1) {
        glDeleteTextures(1, &m_textureId);
        m_textureId = (GLuint)-1;
    }
}
