#pragma once

#include "Engine/GameObject.h"
#include "Engine/Mesh.h"
#include "Engine/Program.h"
#include "Engine/Texture.h"

#include <glm/mat4x4.hpp>
#include <string>

class Camera;

/*
    A class that generates a simple quad with a texture.
*/
class TexturedQuadGameObject : public GameObject
{
public:
    TexturedQuadGameObject(const glm::vec3& pos, Texture texture) :
        GameObject(Transform(pos)),
        m_texture(texture)
    {}

    bool create() override;
    void destroy() override;

    Texture m_texture;
};

