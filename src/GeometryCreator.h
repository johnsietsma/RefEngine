#pragma once


struct Vertex_PositionColor;

/*
	GeometryCreator is a utility class for creating simple geometry primitives.
	It is an absrtact class that is designed to be a holder for static functions that do the geometry creation.
	Creation function allocate memory, it's up to the caller to delete the memory.
*/
class GeometryCreator
{
public:
	GeometryCreator() = delete;

	static void createGrid(int rowCount, int columnCount, Vertex_PositionColor** ppVertexPositionBuffer, unsigned int** ppIndicesBuffer);
};
