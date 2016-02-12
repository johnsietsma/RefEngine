#pragma once

#include "Mesh.h"
#include "Program.h"
#include "Transform.h"

#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>

/*
	A class that generates a simple grid.
*/
class VertexColoredGrid
{
public:
	bool create( const glm::vec3& pos, const glm::ivec2& gridSize );
	void destroy();

	void draw(const glm::mat4& projectionViewMatrix);

private:
	Mesh m_mesh;
	Program m_program;
	Transform m_transform;
};

