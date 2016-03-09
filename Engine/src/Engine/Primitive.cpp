#include "Primitive.h"


Primitive::Primitive(   const Buffer& a_buffer, // The buffer with the data 
                        const BufferAccessor& a_bufferAccessor, // 
                        bool a_isStatic
                    ) :
    Primitive( a_buffer, std::vector<BufferAccessor>{ a_bufferAccessor }, a_isStatic )
{}


Primitive::Primitive(   const Buffer& a_buffer,
                        const std::vector<BufferAccessor>& a_bufferAccessor, 
                        bool a_isStatic
                    ) :
    buffer(a_buffer),
    bufferAccessor(a_bufferAccessor),
    isStatic(a_isStatic)
{}
