#pragma once

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include <cstring> // For memcmp

// A complete vertex structure with all the data needed from the FBX file
struct FBXVertex
{
public:
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
