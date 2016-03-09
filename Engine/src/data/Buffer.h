#pragma once

#include <array>
#include <vector>

// Nothing here owns data and is copyable.

// A buffer is simply a pointer some data and its size.
struct Buffer 
{
    const void* data; // no ownership implied, may be null
    size_t size; // byte size

    Buffer() :
        data(nullptr),
        size(0)
    {}

    Buffer(const void* a_data, size_t a_size) :
        data(a_data),
        size(a_size)
    {}
    
    // Helper functions that make Buffers from std containers.
    template<typename T, size_t N>
    static Buffer create(const std::array<T, N>& buffer, int elementsPerComponent=1);
    
    template<typename T>
    static Buffer create(const std::vector<T>& buffer, int elementsPerComponent=1);
    
};


template<typename T, size_t N>
Buffer Buffer::create(const std::array<T, N>& buffer, int elementsPerComponent)
{
    return Buffer( buffer.data(), buffer.size()*sizeof(T) );
}

template<typename T>
Buffer Buffer::create(const std::vector<T>& buffer, int elementsPerComponent)
{
    return Buffer( buffer.data(), buffer.size()*sizeof(T) );
}
