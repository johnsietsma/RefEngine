#include "FBXMeshNode.h"

#include <glm/vec3.hpp>
#include <glm/glm.hpp>

void FBXMeshNode::calculateTangentsAndBinormals()
{
    if ((m_vertexAttributes & VertexAttributeFlags::eTEXCOORD1) == 0) return;
    
    m_vertexAttributes |= VertexAttributeFlags::eTANGENT|VertexAttributeFlags::eBINORMAL;
    
    size_t vertexCount = m_vertices.size();
    glm::vec3* tan1 = new glm::vec3[vertexCount * 2];
    glm::vec3* tan2 = tan1 + vertexCount;
    memset(tan1, 0, vertexCount * sizeof(glm::vec3) * 2);

    size_t indexCount = m_indices.size();
    for (unsigned int a = 0; a < indexCount; a += 3)
    {
        unsigned int i1 = m_indices[a];
        unsigned int i2 = m_indices[a + 1];
        unsigned int i3 = m_indices[a + 2];

        const glm::vec4& v1 = m_vertices[i1].position;
        const glm::vec4& v2 = m_vertices[i2].position;
        const glm::vec4& v3 = m_vertices[i3].position;

        const glm::vec2& w1 = m_vertices[i1].texCoord1;
        const glm::vec2& w2 = m_vertices[i2].texCoord1;
        const glm::vec2& w3 = m_vertices[i3].texCoord1;

        float x1 = v2.x - v1.x;
        float x2 = v3.x - v1.x;
        float y1 = v2.y - v1.y;
        float y2 = v3.y - v1.y;
        float z1 = v2.z - v1.z;
        float z2 = v3.z - v1.z;

        float s1 = w2.x - w1.x;
        float s2 = w3.x - w1.x;
        float t1 = w2.y - w1.y;
        float t2 = w3.y - w1.y;

        float t = s1 * t2 - s2 * t1;
        float r = t == 0 ? 0 : 1.0f / t;
        glm::vec3 sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r);
        glm::vec3 tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r, (s1 * z2 - s2 * z1) * r);

        tan1[i1] += sdir;
        tan1[i2] += sdir;
        tan1[i3] += sdir;

        tan2[i1] += tdir;
        tan2[i2] += tdir;
        tan2[i3] += tdir;
    }

    for (size_t a = 0; a < vertexCount; a++)
    {
        const glm::vec3& n = m_vertices[a].normal.xyz();
        const glm::vec3& t = tan1[a];

        // Gram-Schmidt orthogonalise
        glm::vec3 p = t - n * glm::dot(n, t);
        if ( glm::length(p) != 0 )
        {
            m_vertices[a].tangent = glm::vec4( glm::normalize( p ), 0.0f );

            // calculate binormal
            float sign = glm::dot(glm::cross(n.xyz(), t.xyz()), tan2[a].xyz()) < 0.0f ? -1.0f : 1.0f;
            m_vertices[a].binormal = glm::vec4(glm::cross(m_vertices[a].normal.xyz(),m_vertices[a].tangent.xyz()) * sign, 0);
        }
    }

    delete[] tan1;    
}
