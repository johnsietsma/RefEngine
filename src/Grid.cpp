#include "Grid.h"

#include "gl_core_4_4.h"
#include "GeometryCreator.h"
#include "Helpers.h"
#include "Vertex.h"

#include <glm/vec2.hpp>

#include <assert.h>
#include <string>

bool Grid::create( const glm::ivec2& gridSize )
{

	std::string vertShader = ReadFile("./data/shaders/color.vert");
	if (vertShader.length() == 0) return false;

	std::string fragShader = ReadFile("./data/shaders/vertexColor.frag");
	if (fragShader.length() == 0) return false;

	if (!m_program.create(vertShader.c_str(), fragShader.c_str())) return false;


	Vertex_PositionColor* pGridVertices;
	unsigned int* pIndices;

	GeometryCreator::createGrid(gridSize.x, gridSize.y, &pGridVertices, &pIndices);

	unsigned int vertexCount = gridSize.x * gridSize.y;
	unsigned int indexCount = (gridSize.x - 1) * (gridSize.y - 1) * 6;
	m_mesh.create(pGridVertices, vertexCount, pIndices, indexCount);

	delete[] pGridVertices;
	delete[] pIndices;

	return true;
}


void Grid::destroy()
{
	m_mesh.destroy();
	m_program.destroy();
}


void Grid::draw( const glm::mat4& projectionViewMatrix )
{
	assert(m_program.isValid());
	glUseProgram(m_program.getId());
	m_program.setUniform("projectionView", projectionViewMatrix);

	assert(m_mesh.isValid());
	glBindVertexArray(m_mesh.getVAO());

	glDrawElements(GL_TRIANGLES, m_mesh.getIndexCount(), GL_UNSIGNED_INT, 0);

}