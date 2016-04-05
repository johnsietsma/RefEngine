#include "VertexColoredGridGameObject.h"

#include "data/Vertex.h"

#include "engine/Camera.h"
#include "engine/GeometryCreator.h"
#include "engine/Helpers.h"
#include "engine/ResourceCreator.h"

#include "graphics/Mesh.h"

#include "graphics/RefOpenGL.h"

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
