#pragma once

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include <vector>

/*
    Vertex definitions.
    Add new structs here to define new vertex types.
    If you add a new vertex type add a new template specilization to VertexAttributes.h
*/


struct Vertex_PositionColor
{
    glm::vec4 position;
    glm::vec4 color;
};

struct Vertex_PositionTexCoord
{
    glm::vec4 position;
    glm::vec2 texCoord;
};

struct Vertex_PositionNormalTexCoord
{
    glm::vec4 position;
    glm::vec4 normal;
    glm::vec2 texCoord;
};

struct Vertex_FBX
{
    glm::vec4 position;
    glm::vec4 colour;
    glm::vec4 normal;
    glm::vec4 tangent;
    glm::vec4 binormal;
    glm::vec4 indices;
    glm::vec4 weights;
    glm::vec2 texCoord1;
    glm::vec2 texCoord2;
};

struct Vertices_FBX
{
    std::vector<glm::vec4> position;
    std::vector<glm::vec4> colour;
    std::vector<glm::vec4> normal;
    std::vector<glm::vec4> tangent;
    std::vector<glm::vec4> binormal;
    std::vector<glm::vec4> indices;
    std::vector<glm::vec4> weights;
    std::vector<glm::vec2> texCoord1;
    std::vector<glm::vec2> texCoord2;
};