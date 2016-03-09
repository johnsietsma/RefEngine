#pragma once

#include "engine/GameObject.h"
#include "graphics/Mesh.h"
#include "graphics/Program.h"

#include <glm/vec2.hpp>

class Camera;

/*
	A class that generates a simple grid.
*/
class VertexColoredGridGameObject : public GameObject
{
public:
    VertexColoredGridGameObject(const Transform& trans, const glm::ivec2& gridSize) :
        GameObject(trans),
        m_gridSize(gridSize)
    {}

	bool create() override;
    void update(float deltaTime) override {} // no-op

private:
    glm::ivec2 m_gridSize;
};

