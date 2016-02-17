#pragma once

#include "GameObject.h"
#include "Mesh.h"
#include "Program.h"
#include "Texture.h"

#include <glm/mat4x4.hpp>
#include <string>

class Camera;

/*
	A class that generates a simple quad with a texture.
*/
class TexturedQuad : public GameObject
{
public:
    TexturedQuad(const glm::vec3& pos, const char* pTextureFilename) :
        GameObject(Transform(pos)),
        m_filename(pTextureFilename)
    {}

	bool create() override;
	void destroy() override;

    void update(float deltaTime) override {} //no-op
	void draw( const Camera& camera ) override;

private:
	Mesh m_mesh;
	Program m_program;
	Texture m_texture;
    std::string m_filename;
};

