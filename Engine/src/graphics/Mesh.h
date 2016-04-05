#pragma once


#include "data/BufferAccessor.h"
#include "data/Primitive.h"

#include "RefOpenGL.h"

#include <vector>


/*
    A Mesh is a vertex array object, a collection vertex buffer objects and a vertex index object.
*/
class Mesh
{
public:

    bool isValid() { return m_indexCount > 0 && m_vaoId != (GLuint)-1; }

    GLuint getVAO() const { return m_vaoId; }
    std::vector<GLuint> getVBOs() const { return m_vboIds; }
    size_t getIndexCount() const { return m_indexCount; }

    void setIndexCount(GLsizei indexCount) { m_indexCount = indexCount; }

    void create(const Primitive& primitive, const Buffer& indicesBuffer);
    void create(const std::vector<Primitive>& primitives, const Buffer& indicesBuffer);
    
    void destroy();

private:
    // Vertex array object id
    GLuint m_vaoId = -1;

    // Index buffer information
    GLuint m_iboId = -1;
    size_t m_indexCount = 0;

    // Vertex buffer information
    std::vector<GLuint> m_vboIds;

};


