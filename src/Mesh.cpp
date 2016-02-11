#include "Mesh.h"

#include "Vertex.h"

#include <assert.h>

Mesh::~Mesh()
{
	assert(m_VAO == -1 && "Mesh has not been destroyed.");
}


bool Mesh::create(Vertex_PositionColor* pVertices, unsigned int vertexCount, unsigned int* pIndices, unsigned int indexCount)
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

	// Position vertex attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex_PositionColor), (void*)(offsetof(Vertex_PositionColor, position)));

	// Color vertex attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex_PositionColor), (void*)(offsetof(Vertex_PositionColor, color)));

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


void Mesh::destroy()
{
	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(1, &m_VBO);
	glDeleteBuffers(1, &m_IBO);

	m_VAO = (GLuint)-1;
}