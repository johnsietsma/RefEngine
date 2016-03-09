#pragma once

#include "Buffer.h"
#include "BufferAccessor.h"

#include "VertexAttributes.h"


// A Primitive wraps a buffer and its attributes.
// A buffer is a data blob. The accessors provides ways to pull data out of the blob.
struct Primitive
{
    // The default, empty index buffer
    static const std::vector<unsigned int> EmptyIndex;

    Primitive(  const Buffer& a_buffer, // The buffer with the data 
                const BufferAccessor& a_bufferAccessor, // 
                bool a_isStatic = true
            );

    Primitive(  const Buffer& a_buffer, // The buffer with the data 
                const std::vector<BufferAccessor>& a_bufferAccessor, // 
                bool a_isStatic = true
            );
            
            
    
    // Helper functions that make Primitives from std containers.
    template<typename T, size_t N>
    static Primitive create(const std::array<T, N>& buffer, int elementsPerComponent=1);
    
    template<typename T>
    static Primitive create(const std::vector<T>& buffer, int elementsPerComponent=1);
    
    template<typename T>
    static Primitive create(const T* pBufferData, size_t bufferSize, int elementsPerComponent=1);

    Buffer buffer;
    const std::vector<BufferAccessor> bufferAccessor;
    const bool isStatic;
};


// -----------------------------



template<typename T, size_t N>
Primitive Primitive::create(const std::array<T, N>& buffer, int elementsPerComponent)
{
    return create( buffer.data(), buffer.size(), elementsPerComponent );
}

template<typename T>
Primitive Primitive::create(const std::vector<T>& buffer, int elementsPerComponent)
{
    return create( buffer.data(), buffer.size(), elementsPerComponent );
}

template<typename T>
Primitive Primitive::create(const T* pBufferData, size_t bufferSize, int elementsPerComponent)
{
    return Primitive(
        Buffer( pBufferData, bufferSize * sizeof(T) * elementsPerComponent ),
        VertexAttributes::create<T>()  
    ); 
}