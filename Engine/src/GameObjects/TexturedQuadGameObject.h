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
    TexturedQuadGameObject(const Transform& trans, Texture texture, const char* pFragShaderName="textured") :
        GameObject(trans),
        m_texture(texture),
        m_fragShaderName( pFragShaderName )
    {}

    bool create() override;

private:
    Texture m_texture;
    std::string m_fragShaderName;
};

