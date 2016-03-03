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

void addParticles(std::shared_ptr<Engine> pEngine)
{
    ParticleEmitterConfig config;
    config.emitRate = 500;
    config.startColor = glm::vec4(1, 0, 0, 1);
    config.endColor = glm::vec4(1, 1, 0, 1);
    config.lifespanMin = 0.1f;
    config.lifespanMax = 5;
    config.particleCount = 1000;
    config.startSize = 1.f;
    config.endSize = 0.1f;
    config.velocityMin = 0.1f;
    config.velocityMax = 1.0f;
    pEngine->addGameObject(std::make_shared<ParticleEmitterGameObject>(config, pEngine->getMainCamera()));
}

bool addSoulSpear(std::shared_ptr<Engine> pEngine)
{
    // Programs that the RenderMode component can swap between,
    Program vertLitProgram = ResourceCreator::CreateProgram("./data/shaders/texNormal.vert", "./data/shaders/vertexLit.frag");
    if (!vertLitProgram.isValid())
        return false;

    Program texturedProgram = ResourceCreator::CreateProgram("./data/shaders/texNormal.vert", "./data/shaders/textured.frag");
    if (!texturedProgram.isValid())
        return false;

    Program fragLitProgram = ResourceCreator::CreateProgram("./data/shaders/normal.vert", "./data/shaders/texturedVertLit.frag");
    if (!fragLitProgram.isValid())
        return false;

    std::vector<Program> renderPrograms{
        vertLitProgram,
        texturedProgram,
        fragLitProgram
    };

    auto fbxGameObject = std::make_shared<FBXMeshGameObject>(Transform(), "./data/models/soulspear/soulspear.fbx", nullptr);
    fbxGameObject->addComponent(std::make_shared<RenderModeComponent>(fbxGameObject, renderPrograms));
    pEngine->addGameObject(fbxGameObject);

    return true;
}

bool setup(std::shared_ptr<Engine> pEngine)
{
    // Setup a FBO camera and RenderPass
    Transform camTransform(vec3(2, 10, -10), vec3(0));
    auto pFboCamera = std::make_shared<Camera>(camTransform, glm::radians(45.f), 1.f, 0.1f, 1000.f);
    pEngine->addCamera(pFboCamera);

    int fboDims = 128;
    RenderPass fboRenderPass(pFboCamera, glm::vec3(0.6f,0,0), glm::ivec2(fboDims, fboDims));
    if (!fboRenderPass.create()) {
        return false;
    }

    pEngine->addRenderPass(fboRenderPass);

    // Apply the FBO to a textured quad
    Transform fboQuadTrans(glm::vec3(0, 0.02f, 5));
    pEngine->addGameObject( std::make_shared<TexturedQuadGameObject>(fboQuadTrans, fboRenderPass.getTexture(), "boxBlur") );

    if (!addSoulSpear(pEngine))
        return false;

    Transform pyroTransform = Transform(glm::vec3(-6, 0, -2), glm::quat(), glm::vec3(0.001f));
    pEngine->addGameObject(std::make_shared<FBXMeshGameObject>(pyroTransform, "./data/models/Pyro/pyro.fbx", "PYRO_BASE"));

    Transform spriteSheetTrans(glm::vec3(-5, 2.5f, -5), glm::angleAxis(glm::radians(90.f), Transform::WORLD_RIGHT), glm::vec3(0.5f));
    pEngine->addGameObject(std::make_shared<SpriteSheetQuadGameObject>(spriteSheetTrans, "./data/textures/spritesheet.png", 4, 4));

    Texture quadTexture = ResourceCreator::CreateTexture("./data/textures/crate.png");
    if (!quadTexture.isValid())
        return false;

    Transform quadTrans(glm::vec3(0, 2.5f, -5), glm::angleAxis(glm::radians(90.f), Transform::WORLD_RIGHT), glm::vec3(0.5f));
    pEngine->addGameObject(std::make_shared<TexturedQuadGameObject>(quadTrans, quadTexture));

    Transform vertColorTrans(glm::vec3(5, 2.5f, -5), glm::angleAxis(glm::radians(90.f), Transform::WORLD_RIGHT), glm::vec3(0.5f));
    pEngine->addGameObject(std::make_shared<VertexColoredGridGameObject>(vertColorTrans, glm::ivec2(5, 5)));

    pEngine->addGameObject(std::make_shared<ProceduralGenerationGameObject>(glm::vec3(0)));

    addParticles(pEngine);

    return true;
}

int main() {
    auto pEng = std::make_shared<Engine>("TestBed");

    if( !setup( pEng ) ) 
        return 2;

    if (!pEng->startup()) 
        return 1;

    pEng->run();

    pEng->shutdown();

    return 0;
}

