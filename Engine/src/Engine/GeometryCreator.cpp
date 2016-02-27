#include "GeometryCreator.h"

#include "MeshData.h"
#include "Vertex.h"

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <cmath>


void GeometryCreator::createTexuredQuad(Vertex_PositionTexCoord** ppVertexPositionBuffer, unsigned int** ppIndicesBuffer)
{
	Vertex_PositionTexCoord* pVertices = new Vertex_PositionTexCoord[6 * 4] // 6 floats, 4 verts
	{
		{ glm::vec4(-5, 0,  5,  1), glm::vec2(0, 1) },
		{ glm::vec4( 5, 0,  5,  1), glm::vec2(1, 1) },
		{ glm::vec4( 5, 0, -5,  1), glm::vec2(1, 0) },
		{ glm::vec4(-5, 0, -5,  1), glm::vec2(0, 0) }
	};

	*ppVertexPositionBuffer = pVertices;

	unsigned int* pIndices = new unsigned int[6]
	{
		0, 1, 2,
		0, 2, 3,
	};

	*ppIndicesBuffer = pIndices;
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
