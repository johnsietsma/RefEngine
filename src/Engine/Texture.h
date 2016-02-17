#pragma once

#include "gl_core_4_4.h"

#include <assert.h>


/*
	Represents a texture owned by OpenGL.
*/
class Texture
{
public:
	Texture() : m_textureId((GLuint)-1) {}
	~Texture() = default;

	bool isValid() { return m_textureId != (GLuint)-1; }
	GLuint getId() const { return m_textureId; }

	// Create and upload the OpenGL texture using the data provided.
	void create(const unsigned char* data, int imageWidth, int imageHeight);
	void destroy();

private:
	GLuint m_textureId;
};
