#include "GeometryCreator.h"

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <array>
#include <cmath>


template<>
Mesh GeometryCreator::createTexturedQuad<Vertex_PositionTexCoord>()
{
    const int size = 10;
    const size_t vertexCount = 6 * 4; // 6 floats, 4 verts
    std::array<Vertex_PositionTexCoord, vertexCount> vertices =
    { {
        { glm::vec4(-size, 0,  size,  1), glm::vec2(0, 1) },
        { glm::vec4( size, 0,  size,  1), glm::vec2(1, 1) },
        { glm::vec4( size, 0, -size,  1), glm::vec2(1, 0) },
        { glm::vec4(-size, 0, -size,  1), glm::vec2(0, 0) }
    } };

    const size_t indexCount = 6; // 2 tris per qu}ad
    std::array<GLuint, indexCount> indices  
    { {
        0, 1, 2,
        0, 2, 3,
    } };
    
    Mesh mesh;
    mesh.create(
        Primitive::create(vertices, 1),
        Buffer::create(indices, 1)
     );
    return mesh;
}


template<>
Mesh GeometryCreator::createTexturedQuad<Vertex_PositionNormalTexCoord>()
{
    const int size = 10;
    const size_t vertexCount = 10 * 4; // 10 floats, 4 verts
    std::array<Vertex_PositionNormalTexCoord, vertexCount> vertices =
    { {
        { glm::vec4(-size, 0,  size,  1), glm::vec4(0,1,0,0), glm::vec2(0, 1) },
        { glm::vec4( size, 0,  size,  1), glm::vec4(0,1,0,0), glm::vec2(1, 1) },
        { glm::vec4( size, 0, -size,  1), glm::vec4(0,1,0,0), glm::vec2(1, 0) },
        { glm::vec4(-size, 0, -size,  1), glm::vec4(0,1,0,0), glm::vec2(0, 0) }
    } };

    const size_t indexCount = 6; // 2 tris per quad
    std::array<GLuint, indexCount> indices  
    { {
        0, 1, 2,
        0, 2, 3,
    } };

    Mesh mesh;
    mesh.create(
        Primitive::create(vertices, 1),
        Buffer::create(indices, 1)
     );
    return mesh;

}


void GeometryCreator::fillGridData(std::vector<Vertex_PositionColor>& vertices, unsigned int rowCount, unsigned int columnCount)
{
    for (unsigned int rowIndex = 0; rowIndex < rowCount; rowIndex++)
    {
        for (unsigned int columnIndex = 0; columnIndex < columnCount; columnIndex++)
        {
            int index = rowIndex * columnCount + columnIndex;
            glm::vec3 color = glm::vec3(sinf(columnIndex / (columnCount - 1.f) * rowIndex / (rowCount - 1.f)));
            vertices[index].color = glm::vec4(color, 1);
        }
    }
}


void GeometryCreator::fillGridData(std::vector<Vertex_PositionTexCoord>& vertices, unsigned int rowCount, unsigned int columnCount)
{
    for (unsigned int rowIndex = 0; rowIndex < rowCount; rowIndex++)
    {
        for (unsigned int columnIndex = 0; columnIndex < columnCount; columnIndex++) 
        {
            // Normalize the row position to get it in a 0 to 1 range
            glm::vec2 texCoord(rowIndex / static_cast<float>(rowCount), columnIndex / static_cast<float>(columnCount));

            int index = rowIndex * columnCount + columnIndex;
            vertices[index].texCoord = texCoord;
        }
    }
}
