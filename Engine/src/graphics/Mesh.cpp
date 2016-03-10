#include "Mesh.h"

#include "data/Vertex.h"

#include <assert.h>


void Mesh::create(const Primitive& primitive, const Buffer& indicesBuffer)
{
    create( std::vector<Primitive>{primitive}, indicesBuffer );
}

void Mesh::create(const std::vector<Primitive>& primitives, const Buffer& indicesBuffer)
{
    assert(!isValid());

    m_vboIds.reserve(primitives.size());

    m_indexCount = indicesBuffer.size / sizeof(unsigned int);

    // Make the VAO
    glGenVertexArrays(1, &m_vaoId);
	glBindVertexArray(m_vaoId);

    // Optionally make the IBO
    if (m_indexCount > 0) {
        assert(indicesBuffer.data);
        glGenBuffers(1, &m_iboId);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iboId);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesBuffer.size, indicesBuffer.data, GL_STATIC_DRAW);
    }

    // Run through each buffer, create and upload it.
    m_vboIds.resize(primitives.size());
    for (size_t i = 0; i < primitives.size(); i++)
    {
        auto& prim = primitives[i];

        auto& buffer = prim.buffer;

        // Check preconditions
        assert(buffer.size > 0);
        assert(buffer.data != nullptr);

        GLenum usage = prim.isStatic ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW;

        // Make the VBO and upload data

        glGenBuffers(1, &m_vboIds[i]);
        glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[i]);
        glBufferData(GL_ARRAY_BUFFER, buffer.size, buffer.data, usage);

        size_t totalOffset = 0;

        // Set up our vertex attributes
        for (size_t i = 0; i < prim.bufferAccessor.size(); i++) {
            auto attrib = prim.bufferAccessor[i];

            // If there is no offset, use the calculated one
            size_t offset = attrib.byteOffset == 0 ? totalOffset : attrib.byteOffset;

            GLuint attribLocation = (GLuint)attrib.accessorType; // Use the type as the location
            glEnableVertexAttribArray(attribLocation);
            
            // We get the stride from the Primitive, not the attribute.
            glVertexAttribPointer(attribLocation, attrib.count, attrib.dataType, GL_FALSE, attrib.byteStride, ((char*)0) + offset);

            // If not offset is given, calculate it
            totalOffset += attrib.byteOffset == 0 ? attrib.count*attrib.byteStride : attrib.byteOffset;
        }
    }

    // Clear the VAO binding
    glBindVertexArray(0);
}


void Mesh::destroy()
{
    glDeleteVertexArrays(1, &m_vaoId);
    for( auto& vboId : m_vboIds ) {
        glDeleteBuffers(1, &vboId);
    }
    glDeleteBuffers(1, &m_iboId);

    m_vaoId = (GLuint)-1;
}
