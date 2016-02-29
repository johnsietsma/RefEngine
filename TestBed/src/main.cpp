#include "config.h"

#if defined USE_VLD
#include <vld.h>
#endif

#include "Engine.h"
#include "GameObjects.h"
#include "Window.h"

#include "Components/RenderModeComponent.h"

#include "Engine/Camera.h"
#include "Engine/RenderPass.h"
#include "Engine/ResourceCreator.h"

#include <glm/vec3.hpp>
#include <memory>

using glm::vec3;

bool setup(std::shared_ptr<Engine> pEngine)
{
    // Setup a FBO camera and RenderPass
    Transform camTransform(vec3(2, 10, -10), vec3(0));
    auto pFboCamera = std::make_shared<Camera>(camTransform, glm::radians(45.f), 1.f, 0.1f, 1000.f);
    pEngine->addCamera(pFboCamera);

    RenderPass fboRenderPass(pFboCamera, glm::vec3(0.6f,0,0));
    if (!fboRenderPass.create()) {
        return false;
    }

    pEngine->addRenderPass(fboRenderPass);

    pEngine->addGameObject( std::make_shared<TexturedQuadGameObject>(glm::vec3(3, 0.02f, -3), fboRenderPass.getTexture()) );


    //Transform pyroTransform = Transform(glm::vec3(0, 0, -2), glm::quat(), glm::vec3(0.01f));

    /*ParticleEmitterConfig config;
    config.emitRate = 500;
    config.startColor = glm::vec4(1, 0, 0, 1);
    config.endColor = glm::vec4(1, 1, 0, 1);
    config.lifespanMin = 0.1f;
    config.lifespanMax = 5;
    config.particleCount = 1000;
    config.startSize = 1.f;
    config.endSize = 0.1f;
    config.velocityMin = 0.1f;
    config.velocityMax = 1.0f;*/


    Program vertLitProgram = ResourceCreator::CreateProgram("./data/shaders/texNormal.vert", "./data/shaders/vertexLit.frag");
    if( !vertLitProgram.isValid() )
        return false;

    Program texturedProgram = ResourceCreator::CreateProgram("./data/shaders/texNormal.vert", "./data/shaders/textured.frag");
    if( !texturedProgram.isValid() )
        return false;

    Program fragLitProgram = ResourceCreator::CreateProgram("./data/shaders/fragLit.vert", "./data/shaders/texturedVertLit.frag");
    if( !fragLitProgram.isValid() )
        return false;

    std::vector<Program> renderPrograms {
        vertLitProgram,
        texturedProgram,
        fragLitProgram
    };

    auto fbxGameObject = std::make_shared<FBXMeshGameObject>(Transform(), "./data/models/soulspear/soulspear.fbx", nullptr);
    fbxGameObject->addComponent( std::make_shared<RenderModeComponent>( fbxGameObject, renderPrograms ) );
    pEngine->addGameObject( fbxGameObject );
    //m_gameObjects.emplace_back(std::make_shared<FBXMeshGameObject>(pyroTransform, "./data/models/Pyro/pyro.fbx", nullptr));
    //m_gameObjects.emplace_back(std::make_shared<ParticleEmitterGameObject>(config, m_pCamera.get()));

    //m_gameObjects.emplace_back(std::make_shared<SpriteSheetQuadGameObject>(glm::vec3(-3, 0.02f, -3), "./data/textures/spritesheet.png", 4, 4));

    //Texture quadTexture = ResourceCreator::CreateTexture("./data/textures/crate.png");
    //if (!quadTexture.isValid())
        //return false;
    //pEngine->addGameObject( std::make_shared<TexturedQuadGameObject>(glm::vec3(3, 0.02f, -3), quadTexture) );
    //m_gameObjects.emplace_back(std::make_shared<VertexColoredGridGameObject>(glm::vec3(0, 0.01f, 2), glm::ivec2(5, 5)));
    //m_gameObjects.emplace_back(std::make_shared<ProceduralGenerationGameObject>(glm::vec3(0)));

    return true;
}

int main() {
    auto pEng = std::make_shared<Engine>("TestBed");

    if( !setup( pEng ) ) return 2;

    if (!pEng->startup()) return 1;

    pEng->run();

    pEng->shutdown();

    return 0;
}
