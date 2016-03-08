#pragma once

#include "Vertex.h"

#include "gl_core_4_4.h"

#include <assert.h>
#include <vector>


// Follows: https://www.opengl.org/sdk/docs/man/html/glVertexAttribPointer.xhtml
struct VertexAttribute
{
    GLuint index; // The index/location of the vertex attribute.
    GLint size; // The number of components per vertex attribute. 1, 2, 3 or 4.
    GLenum type; // The data type for each component.
    GLsizei stride; // The byte offset between vertex attributes.
    size_t offset; // The offset of the first attribute in the buffer.
};

struct VertexAttributes {
    // Specialize this template to create custom vertex attribute for your struct.
    template<typename T>
    static std::vector<VertexAttribute> Create() { assert(false); }
};


template<>
std::vector<VertexAttribute> VertexAttributes::Create<Vertex_PositionColor>();

template<>
std::vector<VertexAttribute> VertexAttributes::Create<Vertex_PositionTexCoord>();

template<>
std::vector<VertexAttribute> VertexAttributes::Create<Vertex_PositionNormalTexCoord>();

template<>
std::vector<VertexAttribute> VertexAttributes::Create<Vertex_FBX>();
