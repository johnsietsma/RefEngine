#include "Mesh.h"

#include "Vertex.h"

#include <assert.h>

void Mesh::destroy()
{
    glDeleteVertexArrays(1, &m_VAO);
    for( auto& VBO : m_VBOs ) {
        glDeleteBuffers(1, &VBO);
    }
    glDeleteBuffers(1, &m_IBO);

    m_VAO = (GLuint)-1;
}

bool Mesh::create(std::vector<VertexDataBuffer> buffers, unsigned int* pIndexBuffer, GLsizei indexCount)
{
    assert(m_VAO != -1 && "Mesh has already been created.");
    assert(pIndexBuffer != nullptr);
    assert(indexCount != 0);

    // Create the VAO. Must be first so the buffers are associated with it.
    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    // Create the VBOs
    for( auto& buffer : buffers ) 
    {
        assert(buffer.pData != nullptr);
        assert(buffer.dataSize != 0);

        GLuint VBO;
        glGenBuffers(1, &VBO); // Create the VBO
        glBindBuffer(GL_ARRAY_BUFFER, VBO); // Make it active
        glBufferData(GL_ARRAY_BUFFER, buffer.dataSize, buffer.pData, GL_STATIC_DRAW); // Upload data
        
        m_VBOs.push_back(VBO);
        
        for( auto& vertexAttribute : buffer.vertexAttributes )
        {

            glEnable( vertexAttribute.index );
            glVertexAttribPointer(
                vertexAttribute.index, 
                vertexAttribute.size, 
                vertexAttribute.type, 
                GL_FALSE, // Don't normalise
                vertexAttribute.stride, 
                (void*)vertexAttribute.offset
            );
        }
    }

    // Create the IBO
    glGenBuffers(1, &m_IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount*sizeof(unsigned int), pIndexBuffer, GL_STATIC_DRAW);

    // Unbind
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    m_indexCount = indexCount;

    return true;
}

