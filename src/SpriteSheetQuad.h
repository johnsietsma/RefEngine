#pragma once

#include "Mesh.h"
#include "Program.h"
#include "Texture.h"
#include "Transform.h"

#include <glm/mat4x4.hpp>
#include <iostream>

/*
	A class that generates a simple quad with a shader that runs through frames of a sprite sheet.
*/
class SpriteSheetQuad
{
public:
	SpriteSheetQuad() : 
		m_cellIndex(0), 
		m_elapsedTime(0)
	{}

	bool create( const glm::vec3& pos, const char* pSpriteSheetFilename, int cellCountX, int cellCountY );
	void destroy();

	void update(float deltaTime) 
	{
		m_elapsedTime += deltaTime;


		const int timeMultiplier = 10;
		float strectchTime = m_elapsedTime * timeMultiplier;
		if (strectchTime - ((int)strectchTime) < deltaTime * timeMultiplier) {
			m_cellIndex++;
			m_cellIndex %= m_cellCountX * m_cellCountY;
		}
	}

	void draw(const glm::mat4& projectionViewMatrix);

private:
	Mesh m_mesh;
	Program m_program;
	Transform m_transform;
	Texture m_texture;

	int m_cellCountX;
	int m_cellCountY;
	int m_cellIndex = 0;

	float m_elapsedTime;
};

