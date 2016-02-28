#pragma once

#include "Engine/GameObject.h"
#include "Engine/Mesh.h"
#include "Engine/Program.h"

#include <glm/vec2.hpp>

class Camera;

/*
	A class that generates a simple grid.
*/
class VertexColoredGridGameObject : public GameObject
{
public:
    VertexColoredGridGameObject(const glm::vec3& pos, const glm::ivec2& gridSize) :
        GameObject(pos),
        m_gridSize(gridSize)
    {}

	bool create() override;
	void destroy() override;

    void update(float deltaTime) override {} // no-op
	void draw( const Camera& camera, const Light& light ) override;

private:
	Mesh m_mesh;
	Program m_program;
    glm::ivec2 m_gridSize;
};

