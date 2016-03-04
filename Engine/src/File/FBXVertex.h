#pragma once

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include <cstring> // For memcmp

// A complete vertex structure with all the data needed from the FBX file
struct FBXVertex
{
public:

    enum VertexAttributeFlags
    {
        ePOSITION = (1 << 0),
        eCOLOUR = (1 << 1),
        eNORMAL = (1 << 2),
        eTANGENT = (1 << 3),
        eBINORMAL = (1 << 4),
        eINDICES = (1 << 5),
        eWEIGHTS = (1 << 6),
        eTEXCOORD1 = (1 << 7),
        eTEXCOORD2 = (1 << 8),
    };


    FBXVertex();
    ~FBXVertex() = default;

    glm::vec4   position;
    glm::vec4   colour;
    glm::vec4   normal;
    glm::vec4   tangent;
    glm::vec4   binormal;
    glm::vec4   indices;
    glm::vec4   weights;
    glm::vec2   texCoord1;
    glm::vec2   texCoord2;

    bool operator == (const FBXVertex& a_rhs) const;
    bool operator < (const FBXVertex& a_rhs) const;

    // internal use only!
    unsigned int index[4];
};

enum class FbxOffset
{
    PositionOffset = 0,
    ColourOffset = offsetof(FBXVertex, colour),
    NormalOffset = offsetof(FBXVertex, normal),
    TangentOffset = offsetof(FBXVertex, tangent),
    BiNormalOffset = offsetof(FBXVertex, binormal),
    IndicesOffset = offsetof(FBXVertex, indices),
    WeightsOffset = offsetof(FBXVertex, weights),
    TexCoord1Offset = offsetof(FBXVertex, texCoord1),
    TexCoord2Offset = offsetof(FBXVertex, texCoord2)
};


inline FBXVertex::FBXVertex()
    : position(0, 0, 0, 1),
    colour(1, 1, 1, 1),
    normal(0, 0, 0, 0),
    tangent(0, 0, 0, 0),
    binormal(0, 0, 0, 0),
    indices(0, 0, 0, 0),
    weights(0, 0, 0, 0),
    texCoord1(0, 0),
    texCoord2(0, 0)
{

}

inline bool FBXVertex::operator == (const FBXVertex& a_rhs) const
{
    return memcmp(this, &a_rhs, sizeof(FBXVertex)) == 0;
}

inline bool FBXVertex::operator < (const FBXVertex& a_rhs) const
{
    return memcmp(this, &a_rhs, sizeof(FBXVertex)) < 0;
}
