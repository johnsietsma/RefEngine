#include "GeometryCreator.h"

#include "Vertex.h"

#include <glm\vec3.hpp>
#include <glm\vec4.hpp>

#include <cmath>

void GeometryCreator::createGrid(int rowCount, int columnCount, Vertex_PositionColor** ppVertexPositionBuffer, unsigned int** ppIndicesBuffer)
{
	Vertex_PositionColor* pVertices = new Vertex_PositionColor[rowCount * columnCount];
	*ppVertexPositionBuffer = pVertices;

	for (int rowIndex=0; rowIndex < rowCount; rowIndex++)
	{
		for (int columnIndex = 0; columnIndex < columnCount; columnIndex++)
		{
			int index = rowIndex * columnCount + columnIndex;
			pVertices[index].position = glm::vec4((float)columnIndex, 0, (float)rowIndex, 1);
			glm::vec3 color = glm::vec3( sinf( columnIndex/(columnCount-1.f) * rowIndex/(rowCount-1.f) ) );
			pVertices[index].color = glm::vec4(color, 1);
		}
	}

	unsigned int indexCount = (rowCount - 1) * (columnCount - 1) * 6;
	unsigned int* pIndices = new unsigned int[indexCount];
	*ppIndicesBuffer = pIndices;

	unsigned int index = 0;
	for (int rowIndex = 0; rowIndex < (rowCount-1); rowIndex++)
	{
		for (int columnIndex = 0; columnIndex < (columnCount-1); columnIndex++)
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

}
