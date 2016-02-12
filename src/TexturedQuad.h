#pragma once

#include "Mesh.h"
#include "Program.h"
#include "Texture.h"
#include "Transform.h"

#include <glm/mat4x4.hpp>

/*
	A class that generates a simple quad with a texture.
*/
class TexturedQuad
{
public:
	bool create( const glm::vec3& pos );
	void destroy();

	void draw(const glm::mat4& projectionViewMatrix);

private:
	Mesh m_mesh;
	Program m_program;
	Transform m_transform;
	Texture m_texture;
};

