#pragma once

#include "graphics/Mesh.h"
#include "data/Vertex.h"

#include <glm/vec3.hpp>

#include <vector>

struct Vertex_PosivtionColor;
struct Vertex_PosivtionTexCoord;

/*
    GeometryCreator is a utility class for creating simple geometry primitives.
    It is an absrtact class that is designed to be a holder for static functions that do the geometry creation.
    Creation function allocate memory, it's up to the caller to delete the memory.
*/
class GeometryCreator
{
public:
    GeometryCreator() = delete;

    // Create the vertices and indices for a quad.
    template<typename T>
    static Mesh createTexturedQuad() { assert(false) ; }

    // Create the vertices and indices for a grid.
    template<typename T>
    static Mesh createGrid(unsigned int rowCount, unsigned int columnCount, float scale=1);

private:
    // -- Helpers to fill in grid data --

    // Fill in vertex colors across the grid
    static void fillGridData(std::vector<Vertex_PositionColor>& vertices, unsigned int rowCount, unsigned int columnCount);

    // Setup texture coordinates so a texture will stretch across the grid
    static void fillGridData(std::vector<Vertex_PositionTexCoord>& vertices, unsigned int rowCount, unsigned int columnCount);

};


template<>
Mesh GeometryCreator::createTexturedQuad<Vertex_PositionTexCoord>();

template<>
Mesh GeometryCreator::createTexturedQuad<Vertex_PositionNormalTexCoord>();



template<typename T>
inline Mesh GeometryCreator::createGrid(unsigned int rowCount, unsigned int columnCount, float scale)
{
    // Create the verts
    const unsigned int vertexCount = rowCount * columnCount;
    std::vector<T> vertices(vertexCount);

    float width = columnCount * scale;
    float height = rowCount * scale;
    float halfWidth = width / 2.f;
    float halfHeight = height / 2.f;
    const glm::vec3 originPos(-halfWidth, 0, -halfHeight);

    for (unsigned int rowIndex = 0; rowIndex < rowCount; rowIndex++)
    {
        for (unsigned int columnIndex = 0; columnIndex < columnCount; columnIndex++)
        {
            int index = rowIndex * columnCount + columnIndex;

            float columnPos = static_cast<float>(columnIndex) * scale;
            float rowPos = static_cast<float>(rowIndex) * scale;
            glm::vec3 pos(columnPos, 0, rowPos);
            vertices[index].position = glm::vec4((originPos + pos), 1);

        }
    }

    fillGridData(vertices, rowCount, columnCount);

    // Create the indices
    const size_t indexCount = (rowCount - 1) * (columnCount - 1) * 6;
    std::vector<unsigned int> indices(indexCount);

    unsigned int index = 0;
    for (unsigned int rowIndex = 0; rowIndex < (rowCount - 1); rowIndex++)
    {
        for (unsigned int columnIndex = 0; columnIndex < (columnCount - 1); columnIndex++)
        {
            unsigned int indexCurrent = rowIndex * columnCount + columnIndex;
            unsigned int indexNextRow = (rowIndex + 1) * columnCount + columnIndex;
            unsigned int indexNextColumn = indexCurrent + 1;
            unsigned int indexNextRowNextColumn = indexNextRow + 1;

            assert(index + 6 <= indexCount);

            // Triangle 1
            indices[index++] = indexCurrent;
            indices[index++] = indexNextRow;
            indices[index++] = indexNextRowNextColumn;

            // Triangle 2
            indices[index++] = indexCurrent;
            indices[index++] = indexNextRowNextColumn;
            indices[index++] = indexNextColumn;
        }
    }

    // Setup the mesh data
    Mesh mesh;
    mesh.create(
        Primitive::create(vertices, 1),
        Buffer::create(indices, 1)
     );
    return mesh;

}
