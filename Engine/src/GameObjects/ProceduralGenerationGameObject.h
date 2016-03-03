#pragma once

#include "Engine/GameObject.h"
#include "Engine/Mesh.h"
#include "Engine/Program.h"
#include "Engine/Texture.h"

#include <glm/vec3.hpp>

class ProceduralGenerationGameObject : public GameObject
{
public:
    ProceduralGenerationGameObject( const glm::vec3& pos );
    ~ProceduralGenerationGameObject();

    bool create() override;

private:
    Texture m_heightMapTexture;
    const int m_heightMapTextureUnit = 0;
    Texture m_diffuseTexture;
    const int m_diffuseTextureUnit = 1;
};

