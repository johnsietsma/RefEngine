#include "Texture.h"


void Texture::create(const unsigned char* data, int imageWidth, int imageHeight)
{
	assert(m_textureId == (GLuint)-1);

	glGenTextures(1, &m_textureId);
	glBindTexture(GL_TEXTURE_2D, m_textureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

void Texture::destroy()
{
	if (m_textureId != (GLuint)-1) {
		glDeleteTextures(1, &m_textureId);
		m_textureId = (GLuint)-1;
	}
}