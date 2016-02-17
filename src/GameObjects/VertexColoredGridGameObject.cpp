#include "VertexColoredGridGameObject.h"

#include "Engine/Camera.h"
#include "Engine/GeometryCreator.h"
#include "Engine/Helpers.h"
#include "Engine/Vertex.h"

#include "gl_core_4_4.h"

#include <glm/vec2.hpp>

#include <assert.h>
#include <string>

bool VertexColoredGridGameObject::create()
{
	std::string vertShader = ReadFile("./data/shaders/color.vert");
	if (vertShader.length() == 0) return false;

	std::string fragShader = ReadFile("./data/shaders/vertexColor.frag");
	if (fragShader.length() == 0) return false;

	if (!m_program.create(vertShader.c_str(), fragShader.c_str())) return false;


	Vertex_PositionColor* pGridVertices;
	unsigned int* pIndices;

	GeometryCreator::createGrid(m_gridSize.x, m_gridSize.y, &pGridVertices, &pIndices);

	unsigned int vertexCount = m_gridSize.x * m_gridSize.y;
	unsigned int indexCount = (m_gridSize.x - 1) * (m_gridSize.y - 1) * 6;
	m_mesh.create(pGridVertices, vertexCount, pIndices, indexCount);

	delete[] pGridVertices;
	delete[] pIndices;

	return true;
}


void VertexColoredGridGameObject::destroy()
{
	m_mesh.destroy();
	m_program.destroy();
}


void VertexColoredGridGameObject::draw( const Camera& camera )
{
	assert(m_program.isValid());
	glUseProgram(m_program.getId());
	m_program.setUniform("projectionView", camera.getProjectionView() * getTransform().GetMatrix());

	assert(m_mesh.isValid());
	glBindVertexArray(m_mesh.getVAO());

	glDrawElements(GL_TRIANGLES, m_mesh.getIndexCount(), GL_UNSIGNED_INT, 0);

}