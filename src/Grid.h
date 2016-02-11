#pragma once

#include "Mesh.h"
#include "Program.h"

#include <glm/vec2.hpp>

/*
	A class that generates a simple grid.
*/
class Grid
{
public:
	bool create( const glm::ivec2& gridSize );
	void destroy();

	void draw(const glm::mat4& projectionViewMatrix);

private:
	Mesh m_mesh;
	Program m_program;
};

