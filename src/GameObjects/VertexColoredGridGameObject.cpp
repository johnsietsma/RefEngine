#include "VertexColoredGridGameObject.h"

#include "Engine/Camera.h"
#include "Engine/GeometryCreator.h"
#include "Engine/Helpers.h"
#include "Engine/MeshData.h"
#include "Engine/ResourceCreator.h"
#include "Engine/Vertex.h"

#include "gl_core_4_4.h"

#include <glm/vec2.hpp>

#include <assert.h>
#include <string>

bool VertexColoredGridGameObject::create()
{
    m_program = ResourceCreator::CreateProgram( "./data/shaders/color.vert","./data/shaders/vertexColor.frag");
	if (!m_program.isValid()) return false;


    MeshData meshData;
    int rowCount = m_gridSize.x;
    int columnCount = m_gridSize.y;


    // Create and fill in the vertex and index buffers with grid data.
	GeometryCreator::createGrid<Vertex_PositionColor>(&meshData, rowCount, columnCount);

    // Create the OpenGL buffers and upload the vertex and index data.
	m_mesh.create<Vertex_PositionColor>(meshData);

    // Now we've given the buffers to OpenGL, we dont need them anymore.
	delete[] meshData.pVertices;
	delete[] meshData.pIndices;

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