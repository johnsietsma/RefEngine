#pragma once

#include "MeshData.h"
#include "Vertex.h"

#include <glm/vec3.hpp>

struct MeshData;
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
	static MeshData createTexuredQuad(Vertex_PositionTexCoord** ppVertexPositionBuffer, unsigned int** ppIndicesBuffer);
    static MeshData createTexuredQuad(Vertex_PositionNormalTexCoord** ppVertexPositionBuffer, unsigned int** ppIndicesBuffer);

	// Create the vertices and indices for a grid.
    template<typename T>
	static MeshData createGrid(unsigned int rowCount, unsigned int columnCount, float scale=1);

private:
    // -- Helpers to fill in grid data --

    // Doesn't do anything by default
    static void fillGridData(unsigned int rowCount, unsigned int columnCount, void* pVertices) {};

    // Fill in vertex colors across the grid
    static void fillGridData(Vertex_PositionColor* pVertices, unsigned int rowCount, unsigned int columnCount);

    // Setup texture coordinates so a texture will stretch across the grid
    static void fillGridData(Vertex_PositionTexCoord* pVertices, unsigned int rowCount, unsigned int columnCount);

};





template<typename T>
MeshData GeometryCreator::createGrid(unsigned int rowCount, unsigned int columnCount, float scale)
{
    // Create the verts
    unsigned int vertexCount = rowCount * columnCount;
    T* pVertices = new T[vertexCount];

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
            pVertices[index].position = glm::vec4((originPos + pos), 1);

        }
    }

    fillGridData(pVertices, rowCount, columnCount);

    // Create the indices
    unsigned int indexCount = (rowCount - 1) * (columnCount - 1) * 6;
    unsigned int* pIndices = new unsigned int[indexCount];

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
            pIndices[index++] = indexCurrent;
            pIndices[index++] = indexNextRow;
            pIndices[index++] = indexNextRowNextColumn;

            // Triangle 2
            pIndices[index++] = indexCurrent;
            pIndices[index++] = indexNextRowNextColumn;
            pIndices[index++] = indexNextColumn;
        }
    }

    // Setup the mesh data
    return MeshData( pVertices, vertexCount, pIndices, indexCount );
}
