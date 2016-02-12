#pragma once

#include "gl_core_4_4.h"
#include "Vertex.h"

#include <assert.h>




/*
	A Mesh is a vertex array object, a vertex buffer object and a vertex index object.
*/
class Mesh
{
public:
	bool isValid() { return m_indexCount > 0 && m_VAO != (GLuint)-1; }

	GLuint getVAO() const { return m_VAO; }
	GLuint getIndexCount() const { return m_indexCount; }

	// Setup OpenGL buffers and vertex attributes to be able to render these vertices.
	template<typename T>
	bool create(T* pVertices, unsigned int vertexCount, unsigned int* pInidices, unsigned int indexCount);

	void destroy();

private:
	// Specialize this template to do custom vertex attribute setup for your struct.
	template<typename T>
	void SetupVertexAttributes() { assert(false); }

	template<>
	void SetupVertexAttributes<Vertex_PositionColor>() 
	{
		// Position vertex attribute
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex_PositionColor), (void*)(offsetof(Vertex_PositionColor, position)));

		// Color vertex attribute
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex_PositionColor), (void*)(offsetof(Vertex_PositionColor, color)));

	}

	template<>
	void SetupVertexAttributes<Vertex_PositionTexCoord>() 
	{ 
		// Position vertex attribute
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex_PositionTexCoord), (void*)(offsetof(Vertex_PositionTexCoord, position)));

		// Texcoord vertex attribute
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex_PositionTexCoord), (void*)(offsetof(Vertex_PositionTexCoord, uv)));

	}

	unsigned int m_indexCount = 0;
	GLuint m_VAO = (GLuint)-1;
	GLuint m_VBO = (GLuint)-1;
	GLuint m_IBO = (GLuint)-1;
};


template<typename T>
bool Mesh::create(T* pVertices, unsigned int vertexCount, unsigned int* pIndices, unsigned int indexCount)
{
	assert(m_VAO == -1 && "Mesh has already been created.");
	assert(pVertices != nullptr);
	assert(vertexCount != 0);
	assert(pIndices != nullptr);
	assert(indexCount != 0);

	// Create the VAO. Must be first so the buffers are associated with it.
	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	glGenBuffers(1, &m_VBO); // Create the VBO
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO); // Make it active
	glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(Vertex_PositionColor), pVertices, GL_STATIC_DRAW); // Upload data

	SetupVertexAttributes<T>();

	// Create the IBO
	glGenBuffers(1, &m_IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount*sizeof(unsigned int), pIndices, GL_STATIC_DRAW);

	// Unbind
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	m_indexCount = indexCount;

	return true;
}
