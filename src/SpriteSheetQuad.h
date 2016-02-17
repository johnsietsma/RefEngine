#pragma once

#include "GameObject.h"
#include "Mesh.h"
#include "Program.h"
#include "Texture.h"

#include <glm/mat4x4.hpp>
#include <iostream>
#include <string>

class Camera;

/*
	A class that generates a simple quad with a shader that runs through frames of a sprite sheet.
*/
class SpriteSheetQuad : public GameObject
{
public:
    SpriteSheetQuad(const glm::vec3& pos, const char* pSpriteSheetFilename, int cellCountX, int cellCountY);

	bool create() override;
	void destroy() override;

    void update(float deltaTime) override;
	void draw(const Camera& camera) override;

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

