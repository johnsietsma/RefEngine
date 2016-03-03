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
    TexturedQuadGameObject(const Transform& trans, Texture texture, const char* samplerName="diffuseSampler", const char* pVertShaderName = "textured", const char* pFragShaderName="textured") :
        GameObject(trans, 1),
        m_texture(texture),
        m_vertShaderName(pVertShaderName),
        m_fragShaderName(pFragShaderName)
    {}

    bool create() override;

private:
    Texture m_texture;
    std::string m_vertShaderName;
    std::string m_fragShaderName;
};

