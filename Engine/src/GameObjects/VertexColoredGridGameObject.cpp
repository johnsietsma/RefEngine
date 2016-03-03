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
    auto program = ResourceCreator::CreateProgram( "color","vertexColor");
	if (!program.isValid()) return false;

    MeshData meshData;
    int rowCount = m_gridSize.x;
    int columnCount = m_gridSize.y;


    // Create and fill in the vertex and index buffers with grid data.
	GeometryCreator::createGrid<Vertex_PositionColor>(&meshData, rowCount, columnCount);

    // Create the OpenGL buffers and upload the vertex and index data.
    Mesh mesh;
    if (!mesh.create<Vertex_PositionColor>(meshData)) return false;

    // Now we've given the buffers to OpenGL, we dont need them anymore.
	delete[] (Vertex_PositionColor*)meshData.pVertices;
	delete[] meshData.pIndices;

    m_renderables.emplace_back(program, mesh);

	return true;
}
