#pragma once

#include "gl_core_4_4.h"

#include "MeshData.h"
#include "Vertex.h"
#include "VertexAttributes.h"

#include <assert.h>
#include <vector>


/*
    A Mesh is a vertex array object, a collection vertex buffer objects and a vertex index object.
*/
class Mesh
{
public:
    bool isValid() { return m_indexCount > 0 && m_VAO != (GLuint)-1; }

    GLuint getVAO() const { return m_VAO; }
    std::vector<GLuint> getVBOs() const { return m_VBOs; }
    GLsizei getIndexCount() const { return m_indexCount; }

    void setIndexCount(GLsizei indexCount) { m_indexCount = indexCount; }

    // Setup OpenGL buffers and vertex attributes to be able to render these vertices.
    bool create(const MeshData& meshData) {
        return create(meshData.buffers, meshData.pIndexBuffer, meshData.indexCount);
    }

    bool create(std::vector<VertexDataBuffer> buffers, unsigned int* pIndexBuffer, GLsizei indexCount);

    void destroy();

private:
    GLuint m_indexCount = 0;
    GLuint m_VAO = (GLuint)-1;
    GLuint m_IBO = (GLuint)-1;
    std::vector<GLuint> m_VBOs;
};


