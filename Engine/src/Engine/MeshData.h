#pragma once

#include "VertexAttributes.h"

#include "gl_core_4_4.h"
#include <vector>

struct VertexDataBuffer
{
    VertexDataBuffer( const std::vector<VertexAttribute>& a_vertexAttributes, void* a_pData, size_t a_dataSize ) :
        vertexAttributes( a_vertexAttributes ),
        pData( a_pData ),
        dataSize( a_dataSize )
    {}
    
    template<typename T>
    VertexDataBuffer( const std::vector<T>& a_data ) :
        vertexAttributes( VertexAttributes::Create<T>() ),
        pData( a_data.data() ),
        dataSize( a_data.size()*sizeof(T) )
    {}
    
    std::vector<VertexAttribute> vertexAttributes;
    void* pData;
    size_t dataSize;
};

/*
    Contains data that defines the vertex and index data of a mesh.
    This struct does no memory management, that's up to its user.
*/
struct MeshData
{
    template<typename T>
    MeshData( T* a_pData, size_t a_dataCount, unsigned int* a_pIndexBuffer, size_t a_indexCount ) :
        pIndexBuffer( a_pIndexBuffer ),
        indexCount( a_indexCount )
    {
        buffers.emplace_back(
          VertexAttributes::Create<T>(),
          a_pData,
          a_dataCount * sizeof(T)  
        );
    }
    
    // The buffer that contains all the vert data
    std::vector<VertexDataBuffer> buffers;

    // A buffer that contains all the indices
    unsigned int* pIndexBuffer;
    GLsizei indexCount;
};
