#pragma once

#include "engine/GameObject.h"
#include "graphics/Mesh.h"
#include "graphics/Program.h"
#include "graphics/Texture.h"

#include <glm/mat4x4.hpp>
#include <iostream>
#include <string>

class Camera;

/*
	A class that generates a simple quad with a shader that runs through frames of a sprite sheet.
*/
class SpriteSheetQuadGameObject : public GameObject
{
public:
    SpriteSheetQuadGameObject(const Transform& trans, const char* pSpriteSheetFilename, int cellCountX, int cellCountY);

	bool create() override;
    void update(float deltaTime) override;
	void preDraw(const Camera& camera, const Light& light) override;

private:
	int m_cellCountX;
	int m_cellCountY;
	int m_cellIndex = 0;

	float m_elapsedTime;

    std::string m_filename;
};

