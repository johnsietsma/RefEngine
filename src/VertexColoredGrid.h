#pragma once

#include "GameObject.h"
#include "Mesh.h"
#include "Program.h"

#include <glm/vec2.hpp>

class Camera;

/*
	A class that generates a simple grid.
*/
class VertexColoredGrid : public GameObject
{
public:
    VertexColoredGrid(const glm::vec3& pos, const glm::ivec2& gridSize) :
        GameObject(pos),
        m_gridSize(gridSize)
    {}

	bool create() override;
	void destroy() override;

    void update(float deltaTime) {} // no-op
	void draw( const Camera& camera );

private:
	Mesh m_mesh;
	Program m_program;
    glm::ivec2 m_gridSize;
};

