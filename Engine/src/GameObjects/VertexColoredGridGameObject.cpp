#include "VertexColoredGridGameObject.h"

#include "Engine/Camera.h"
#include "Engine/GeometryCreator.h"
#include "Engine/Helpers.h"
#include "Engine/Mesh.h"
#include "Engine/ResourceCreator.h"
#include "Engine/Vertex.h"

#include "gl_core_4_4.h"

#include <glm/vec2.hpp>

#include <assert.h>
#include <string>

bool VertexColoredGridGameObject::create()
{
    auto program = ResourceCreator::createProgram( "color","vertexColor");
    if (!program.isValid()) return false;

    int rowCount = m_gridSize.x;
    int columnCount = m_gridSize.y;

    // Create and fill in the vertex and index buffers with grid data.
    Mesh mesh = GeometryCreator::createGrid<Vertex_PositionColor>(rowCount, columnCount);

    m_renderables.emplace_back(program, mesh);

    return true;
}
