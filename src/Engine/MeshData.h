#pragma once


/*
    Contains data that defeins the vertex and index data of a mesh.
    This struct does no memory management, that's up to its user.
*/
struct MeshData
{
    // A pointer to the buffer that will contain all the verts
    void* pVertices;

    unsigned int vertexCount;

    // A pointer to the buffer that will contain all the indices
    unsigned int* pIndices;

    unsigned int indexCount;
};
