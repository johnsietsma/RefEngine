#include "VertexAttributes.h"

#include "gl_core_4_4.h"

template<>
void VertexAttributes::Setup<Vertex_PositionColor>()
{
    // Position vertex attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex_PositionColor), (void*)(offsetof(Vertex_PositionColor, position)));

    // Color vertex attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex_PositionColor), (void*)(offsetof(Vertex_PositionColor, color)));

}

template<>
void VertexAttributes::Setup<Vertex_PositionTexCoord>()
{
    // Position vertex attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex_PositionTexCoord), (void*)(offsetof(Vertex_PositionTexCoord, position)));

    // Texcoord vertex attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex_PositionTexCoord), (void*)(offsetof(Vertex_PositionTexCoord, texCoord)));

}

template<>
void VertexAttributes::Setup<Vertex_PositionNormalTexCoord>()
{
    // Position vertex attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex_PositionNormalTexCoord), (void*)(offsetof(Vertex_PositionNormalTexCoord, position)));

    // Texcoord vertex attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex_PositionNormalTexCoord), (void*)(offsetof(Vertex_PositionNormalTexCoord, texCoord)));

    // Normal vertex attribute
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex_PositionNormalTexCoord), (void*)(offsetof(Vertex_PositionNormalTexCoord, normal)));
}

template<>
void VertexAttributes::Setup<Vertex_FBX>()
{
    // Position vertex attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex_FBX), (void*)(offsetof(Vertex_FBX, position)));

    // Texcoord vertex attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex_FBX), (void*)(offsetof(Vertex_FBX, texCoord1)));

    // Normal vertex attribute
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex_FBX), (void*)(offsetof(Vertex_FBX, normal)));

    // Tangent vertex attribute
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex_FBX), (void*)(offsetof(Vertex_FBX, tangent)));

    // Weights vertex attribute
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex_FBX), (void*)(offsetof(Vertex_FBX, weights)));

    // Indices vertex attribute
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex_FBX), (void*)(offsetof(Vertex_FBX, indices)));
}
