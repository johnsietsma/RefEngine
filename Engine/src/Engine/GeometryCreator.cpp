#include "GeometryCreator.h"

#include "MeshData.h"
#include "Vertex.h"

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <cmath>


MeshData GeometryCreator::createTexuredQuad(Vertex_PositionTexCoord** ppVertexPositionBuffer, unsigned int** ppIndicesBuffer)
{
    const int size = 10;
    const size_t vertexCount = 6 * 4; // 6 floats, 4 verts
    Vertex_PositionTexCoord* pVertices = new Vertex_PositionTexCoord[vertexCount] 
    {
        { glm::vec4(-size, 0,  size,  1), glm::vec2(0, 1) },
        { glm::vec4( size, 0,  size,  1), glm::vec2(1, 1) },
        { glm::vec4( size, 0, -size,  1), glm::vec2(1, 0) },
        { glm::vec4(-size, 0, -size,  1), glm::vec2(0, 0) }
    };

    size_t indexCount = 6; // 2 tris per quad
    unsigned int* pIndices = new unsigned int[indexCount]
    {
        0, 1, 2,
        0, 2, 3,
    };

    return MeshData( pVertices, vertexCount, pIndices, indexCount );
}


MeshData GeometryCreator::createTexuredQuad(Vertex_PositionNormalTexCoord** ppVertexPositionBuffer, unsigned int** ppIndicesBuffer)
{
    const int size = 10;
    const size_t vertexCount = 10 * 4; // 10 floats, 4 verts
    Vertex_PositionNormalTexCoord* pVertices = new Vertex_PositionNormalTexCoord[vertexCount] 
    {
        { glm::vec4(-size, 0,  size,  1), glm::vec4(0,1,0,0), glm::vec2(0, 1) },
        { glm::vec4( size, 0,  size,  1), glm::vec4(0,1,0,0), glm::vec2(1, 1) },
        { glm::vec4( size, 0, -size,  1), glm::vec4(0,1,0,0), glm::vec2(1, 0) },
        { glm::vec4(-size, 0, -size,  1), glm::vec4(0,1,0,0), glm::vec2(0, 0) }
    };

    *ppVertexPositionBuffer = pVertices;

    size_t indexCount = 6; // 2 tris per quad
    unsigned int* pIndices = new unsigned int[indexCount]
    {
        0, 1, 2,
        0, 2, 3,
    };

    return { pVertices, vertexCount, pIndices, indexCount };
}


void GeometryCreator::fillGridData(Vertex_PositionColor* pVertices, unsigned int rowCount, unsigned int columnCount)
{
    for (unsigned int rowIndex = 0; rowIndex < rowCount; rowIndex++)
    {
        for (unsigned int columnIndex = 0; columnIndex < columnCount; columnIndex++)
        {
            int index = rowIndex * columnCount + columnIndex;
            glm::vec3 color = glm::vec3(sinf(columnIndex / (columnCount - 1.f) * rowIndex / (rowCount - 1.f)));
            pVertices[index].color = glm::vec4(color, 1);
        }
    }
}


void GeometryCreator::fillGridData(Vertex_PositionTexCoord* pVertices, unsigned int rowCount, unsigned int columnCount)
{
    for (unsigned int rowIndex = 0; rowIndex < rowCount; rowIndex++)
    {
        for (unsigned int columnIndex = 0; columnIndex < columnCount; columnIndex++) 
        {
            // Normalize the row position to get it in a 0 to 1 range
            glm::vec2 texCoord(rowIndex / static_cast<float>(rowCount), columnIndex / static_cast<float>(columnCount));

            int index = rowIndex * columnCount + columnIndex;
            pVertices[index].texCoord = texCoord;
        }
    }
}
