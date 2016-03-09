#pragma once

#include "Buffer.h"
#include "BufferAccessor.h"
#include "GLEnum.h"

#include "gl_core_4_4.h"


enum class BufferAccessorType
{
    Position,   // 0
    Color,      // 1
    Normal,     // 2
    Tangent,    // 3
    Binormal,   // 4
    Indices,    // 5 
    Weights,    // 6
    TexCoord1,  // 7
    TexCoord2,  // 8
};

// Based loosely on https://github.com/KhronosGroup/glTF/

// An accessor is used to access typed data in a buffer.
struct BufferAccessor
{
    static const BufferAccessor Empty;

    BufferAccessor() :
        count(0),
        dataType(0),
        byteStride(0),
        byteOffset(0)
    {}

    // Create a new BufferAccessor without a Buffer bound to it. Useful to describe data before it's available.
    BufferAccessor(BufferAccessorType a_accessorType, int a_count, GLenum a_dataType, size_t a_byteStride, size_t a_byteOffset) :
        accessorType(a_accessorType),
        count(a_count),
        dataType(a_dataType),
        byteStride(a_byteStride),
        byteOffset(a_byteOffset)
    {}

    // Helper functions that make BufferAccessors from std containers.
    template<typename T, size_t N>
    static BufferAccessor create(BufferAccessorType a_accessorType, const std::array<T, N>& arrayBuffer, int elementsPerComponent);
    
    template<typename T>
    static BufferAccessor create(BufferAccessorType a_accessorType, const std::vector<T>& vectorBuffer, int elementsPerComponent);

    BufferAccessorType accessorType; // The type we're accessing, eg, positions or normals.
    int count;          // num of attributes (not bytes).
    GLenum dataType;    // The type of data stored in the buffer, eg GL_FLOAT.
    int byteStride;     // Stride of the component in the buffer, eg VEC3 may have a stride of 4*3=12 bytes.
    size_t byteOffset;  // Offset into the buffer.
};


template<typename T, size_t N>
BufferAccessor BufferAccessor::create(BufferAccessorType a_accessorType, const std::array<T, N>& arrayBuffer, int elementsPerComponent)
{
    return BufferAccessor(
        a_accessorType,
        0, // No offset
        sizeof(T) * elementsPerComponent, // The stride is the number of elements * the size of each element
        GLEnum<T>::value, // Lookup the GLenum equiv of the type held in the std::array
        arrayBuffer.size()/elementsPerComponent // Calc the number of components
    );
}    

// A helper function that makes a BufferAccessor from an std::vector.
template<typename T>
BufferAccessor BufferAccessor::create(BufferAccessorType a_accessorType, const std::vector<T>& vectorBuffer, int elementsPerComponent)
{
    return BufferAccessor(
        a_accessorType,
        0,
        sizeof(T) * elementsPerComponent,
        GLEnum<T>::value,
        static_cast<int>(vectorBuffer.size() / elementsPerComponent)
    );
}
