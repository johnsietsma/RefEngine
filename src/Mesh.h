#pragma once

#include "gl_core_4_4.h"

#include <assert.h>

struct Vertex_PositionColor;


/*
	A Mesh is a vertex array object, a vertex buffer object and a vertex index object.
*/
class Mesh
{
public:
	Mesh() = default;
	~Mesh();

	bool isValid() { return m_indexCount > 0 && m_VAO != (GLuint)-1; }

	GLuint getVAO() const { return m_VAO; }
	GLuint getIndexCount() const { return m_indexCount; }

	// Setup OpenGL buffers and vertex attributes to be able to render these vertices.
	// Override this function to support different vertex types.
	bool create(Vertex_PositionColor* pVertices, unsigned int vertexCount, unsigned int* pInidices, unsigned int indexCount);

	void destroy();

private:
	unsigned int m_indexCount = 0;
	GLuint m_VAO = (GLuint)-1;
	GLuint m_VBO = (GLuint)-1;
	GLuint m_IBO = (GLuint)-1;
};

