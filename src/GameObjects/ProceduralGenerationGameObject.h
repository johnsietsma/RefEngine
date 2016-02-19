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
    void destroy() override;

    void update(float deltaTime) override;
    void draw(const Camera& camera) override;

private:
    Mesh m_mesh;
    Program m_program;
    Texture m_heightMapTexture;
};

