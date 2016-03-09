#include "VertexAttributes.h"

#include "gl_core_4_4.h"

template<>
std::vector<BufferAccessor> VertexAttributes::create<Vertex_PositionColor>()
{
    return {
        { BufferAccessorType::Position, 4, GL_FLOAT, sizeof(Vertex_PositionColor), offsetof(Vertex_PositionColor, position) },
        { BufferAccessorType::Color,    4, GL_FLOAT, sizeof(Vertex_PositionColor), offsetof(Vertex_PositionColor, color) }
    };
}

template<>
std::vector<BufferAccessor> VertexAttributes::create<Vertex_PositionTexCoord>()
{
    return {
        { BufferAccessorType::Position,  4, GL_FLOAT, sizeof(Vertex_PositionTexCoord), offsetof(Vertex_PositionTexCoord, position) },
        { BufferAccessorType::TexCoord1, 2, GL_FLOAT, sizeof(Vertex_PositionTexCoord), offsetof(Vertex_PositionTexCoord, texCoord) },
    };
}

template<>
std::vector<BufferAccessor> VertexAttributes::create<Vertex_PositionNormalTexCoord>()
{
    return {
        { BufferAccessorType::Position,  4, GL_FLOAT, sizeof(Vertex_PositionNormalTexCoord), offsetof(Vertex_PositionNormalTexCoord, position) },
        { BufferAccessorType::TexCoord1, 2, GL_FLOAT, sizeof(Vertex_PositionNormalTexCoord), offsetof(Vertex_PositionNormalTexCoord, texCoord) },
        { BufferAccessorType::Normal,    4, GL_FLOAT, sizeof(Vertex_PositionNormalTexCoord), offsetof(Vertex_PositionNormalTexCoord, normal) },
    };
}

template<>
std::vector<BufferAccessor> VertexAttributes::create<Vertex_FBX>()
{
    return {
        { BufferAccessorType::Position,  4, GL_FLOAT, sizeof(Vertex_FBX), offsetof(Vertex_FBX, position) },
        { BufferAccessorType::Color,     4, GL_FLOAT, sizeof(Vertex_FBX), offsetof(Vertex_FBX, color) },
        { BufferAccessorType::Normal,    4, GL_FLOAT, sizeof(Vertex_FBX), offsetof(Vertex_FBX, normal) },
        { BufferAccessorType::Tangent,   4, GL_FLOAT, sizeof(Vertex_FBX), offsetof(Vertex_FBX, tangent) },
        { BufferAccessorType::Binormal,  4, GL_FLOAT, sizeof(Vertex_FBX), offsetof(Vertex_FBX, binormal) },
        { BufferAccessorType::Indices,   4, GL_FLOAT, sizeof(Vertex_FBX), offsetof(Vertex_FBX, indices) },
        { BufferAccessorType::Weights,   4, GL_FLOAT, sizeof(Vertex_FBX), offsetof(Vertex_FBX, weights) },
        { BufferAccessorType::TexCoord1, 2, GL_FLOAT, sizeof(Vertex_FBX), offsetof(Vertex_FBX, texCoord1) },
        { BufferAccessorType::TexCoord2, 2, GL_FLOAT, sizeof(Vertex_FBX), offsetof(Vertex_FBX, texCoord2) },
    };
}

template<>
std::vector<BufferAccessor> VertexAttributes::create<Vertices_FBX>()
{
    return {
        { BufferAccessorType::Position,  4, GL_FLOAT, 0, 0 },
        { BufferAccessorType::Color,     4, GL_FLOAT, 0, 0 },
        { BufferAccessorType::Normal,    4, GL_FLOAT, 0, 0 },
        { BufferAccessorType::Tangent,   4, GL_FLOAT, 0, 0 },
        { BufferAccessorType::Binormal,  4, GL_FLOAT, 0, 0 },
        { BufferAccessorType::Indices,   4, GL_FLOAT, 0, 0 },
        { BufferAccessorType::Weights,   4, GL_FLOAT, 0, 0 },
        { BufferAccessorType::TexCoord1, 2, GL_FLOAT, 0, 0 },
        { BufferAccessorType::TexCoord2, 2, GL_FLOAT, 0, 0 },
    };
}
