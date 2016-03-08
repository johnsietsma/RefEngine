#include "VertexAttributes.h"

#include "gl_core_4_4.h"

template<>
std::vector<VertexAttribute> VertexAttributes::Create<Vertex_PositionColor>()
{
    return {
        // Position
        { 0, 4, GL_FLOAT, sizeof(Vertex_PositionColor), offsetof(Vertex_PositionColor, position) },
        // Color
        { 1, 4, GL_FLOAT, sizeof(Vertex_PositionColor), offsetof(Vertex_PositionColor, color) }
    };
}

template<>
std::vector<VertexAttribute> VertexAttributes::Create<Vertex_PositionTexCoord>()
{
    return {
        // Position
        { 0, 4, GL_FLOAT, sizeof(Vertex_PositionTexCoord), offsetof(Vertex_PositionTexCoord, position) },

        // Texcoord
        { 1, 2, GL_FLOAT, sizeof(Vertex_PositionTexCoord), offsetof(Vertex_PositionTexCoord, texCoord) },
    };
}

template<>
std::vector<VertexAttribute> VertexAttributes::Create<Vertex_PositionNormalTexCoord>()
{
    return {
        // Position
        { 0, 4, GL_FLOAT, sizeof(Vertex_PositionNormalTexCoord), offsetof(Vertex_PositionNormalTexCoord, position) },

        // Texcoord
        { 1, 2, GL_FLOAT, sizeof(Vertex_PositionNormalTexCoord), offsetof(Vertex_PositionNormalTexCoord, texCoord) },

        // Normal
        { 2, 4, GL_FLOAT, sizeof(Vertex_PositionNormalTexCoord), offsetof(Vertex_PositionNormalTexCoord, normal) },
    };
}

template<>
std::vector<VertexAttribute> VertexAttributes::Create<Vertex_FBX>()
{
    return {
        // Position
        { 0, 4, GL_FLOAT, sizeof(Vertex_FBX), offsetof(Vertex_FBX, position) },

        // Texcoord
        { 1, 2, GL_FLOAT, sizeof(Vertex_FBX), offsetof(Vertex_FBX, texCoord1) },

        // Normal
        { 2, 4, GL_FLOAT, sizeof(Vertex_FBX), offsetof(Vertex_FBX, normal) },

        // Tangent
        { 3, 4, GL_FLOAT, sizeof(Vertex_FBX), offsetof(Vertex_FBX, tangent) },

        // Weights
        { 4, 4, GL_FLOAT, sizeof(Vertex_FBX), offsetof(Vertex_FBX, weights) },

        // Indices
        { 5, 4, GL_FLOAT, sizeof(Vertex_FBX), offsetof(Vertex_FBX, indices) },
    };
}
