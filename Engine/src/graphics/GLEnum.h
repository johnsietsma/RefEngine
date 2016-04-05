#pragma once

#include "RefOpenGL.h"

//-----------------------------------------------------------------------------
// Map a C numeric type to a GLenum value at compile time

template<typename T> struct GLEnum {
    static const int value = -1;
};

template<> struct GLEnum<float> {
    static const int value = GL_FLOAT;
};

template<> struct GLEnum<short> {
    static const int value = GL_SHORT;
};

template<> struct GLEnum<int> {
    static const int value = GL_INT;
};

template<> struct GLEnum<unsigned short> {
    static const int value = GL_UNSIGNED_SHORT;
};

template<> struct GLEnum<unsigned int> {
    static const int value = GL_UNSIGNED_INT;
};
