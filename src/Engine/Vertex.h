#pragma once

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

/*
	Vertex definitions.
	Add new structs here to define new vertex types.
	If you add a new vertex type you can add a new Mesh::create override to setup vertex attributes correctly.
*/


struct Vertex_PositionColor
{
	glm::vec4 position;
	glm::vec4 color;
};

struct Vertex_PositionTexCoord
{
	glm::vec4 position;
	glm::vec2 uv;
};
