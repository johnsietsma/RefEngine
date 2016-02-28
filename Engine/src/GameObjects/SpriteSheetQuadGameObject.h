#pragma once

#include "Engine/GameObject.h"
#include "Engine/Mesh.h"
#include "Engine/Program.h"
#include "Engine/Texture.h"

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
    SpriteSheetQuadGameObject(const glm::vec3& pos, const char* pSpriteSheetFilename, int cellCountX, int cellCountY);

	bool create() override;
	void destroy() override;

    void update(float deltaTime) override;
	void draw(const Camera& camera, const Light& light) override;

private:
	Mesh m_mesh;
	Program m_program;
	Texture m_texture;

	int m_cellCountX;
	int m_cellCountY;
	int m_cellIndex = 0;

	float m_elapsedTime;

    std::string m_filename;
};

