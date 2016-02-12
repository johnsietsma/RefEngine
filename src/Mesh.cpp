#include "Mesh.h"

#include "Vertex.h"

#include <assert.h>

void Mesh::destroy()
{
	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(1, &m_VBO);
	glDeleteBuffers(1, &m_IBO);

	m_VAO = (GLuint)-1;
}