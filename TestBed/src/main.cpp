#include "config.h"

#if defined USE_VLD
#include <vld.h>
#endif

#include "Engine.h"
#include "GameObjects.h"
#include "Window.h"

#include "components/RenderModeComponent.h"

#include "engine/Camera.h"
#include "engine/ResourceCreator.h"

#include "graphics/RenderPass.h"

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

    Program vertLitProgram = ResourceCreator::createProgram("normal", "vertexLit");
    if (!vertLitProgram.isValid())
        return false;

    Program texturedProgram = ResourceCreator::createProgram("texturedNormal", "textured");
    if (!texturedProgram.isValid())
        return false;

    Program fragLitProgram = ResourceCreator::createProgram("texturedNormal", "texturedVertexLit");

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

    // Setup a RenderPass
    RenderPassConfig passConfig{
        glm::vec3(0.6f,0,0), // clear color
        glm::ivec2(128), // fbo size
        true, // create color buffer
        true, // create depth buffer
        Program() // Don't override the renderable programs
    };

    RenderPass fboRenderPass(pEngine->getMainCamera(), passConfig);
    if (!fboRenderPass.create())
        return false;

    pEngine->addRenderPass(fboRenderPass);

    // Apply the FBO to a textured quad
    Transform fboQuadTrans(glm::vec3(0, 0, 0));
    pEngine->addGameObject( std::make_shared<TexturedQuadGameObject>(fboQuadTrans, fboRenderPass.getTexture(), "diffuseSampler", "textured", "boxBlur") );

    // --- Soul spear
    if (!addSoulSpear(pEngine))
        return false;

    // --- Pyro ---
    Transform pyroTransform = Transform(glm::vec3(-6, 0, -2), glm::quat(), glm::vec3(0.001f));
    pEngine->addGameObject(std::make_shared<FBXMeshGameObject>(pyroTransform, "./data/models/Pyro/pyro.fbx", "PYRO_BASE"));

    // --- Sprite sheet ---
    Transform spriteSheetTrans(glm::vec3(-5, 2.5f, -5), glm::angleAxis(glm::radians(90.f), Transform::WORLD_RIGHT), glm::vec3(0.2f));
    pEngine->addGameObject(std::make_shared<SpriteSheetQuadGameObject>(spriteSheetTrans, "./data/textures/spritesheet.png", 4, 4));

    // --- Crate ---
    Texture quadTexture = ResourceCreator::createTexture("./data/textures/crate.png");
    if (!quadTexture.isValid())
        return false;

    Transform quadTrans(glm::vec3(0, 2.5f, -5), glm::angleAxis(glm::radians(90.f), Transform::WORLD_RIGHT), glm::vec3(0.2f));
    pEngine->addGameObject(std::make_shared<TexturedQuadGameObject>(quadTrans, quadTexture));

    // --- Vert colored quad ---
    Transform vertColorTrans(glm::vec3(5, 2.5f, -5), glm::angleAxis(glm::radians(90.f), Transform::WORLD_RIGHT), glm::vec3(0.5f));
    pEngine->addGameObject(std::make_shared<VertexColoredGridGameObject>(vertColorTrans, glm::ivec2(5, 5)));

    // --- Terrain ---
    pEngine->addGameObject(std::make_shared<ProceduralGenerationGameObject>(glm::vec3(10,0,0)));


    // --- Particles ---
    addParticles(pEngine);

    // --- Bunny ---
    Transform bunnyTransform = Transform(glm::vec3(-10, 0, 0), glm::quat(), glm::vec3(0.6f));
    pEngine->addGameObject(std::make_shared<FBXMeshGameObject>(bunnyTransform, "./data/models/bunny/Bunny.fbx"));


    // --- Shadow RenderPass, with only depth target ---
    Program shadowProgram = ResourceCreator::createProgram("lightProjection", "depth");
    if (!shadowProgram.isValid())
        return false;

    RenderPassConfig shadowPassConfig{
        glm::vec3(0.2f), // clear color
        glm::ivec2(1024), // fbo size
        false, // no color buffer
        true, // create depth buffer
        shadowProgram,
        1 // Only render things in layer 1
    };
    RenderPass shadowRenderPass(pEngine->getMainCamera(), shadowPassConfig);
    if (!shadowRenderPass.create())
        return false;

    pEngine->addRenderPass(shadowRenderPass);

    // Ground plane for shadow result
    Transform groundTrans(bunnyTransform);
    groundTrans.translate(glm::vec3(0, 0, 0)); // Move the plane centrally under the bunny
    pEngine->addGameObject(std::make_shared<TexturedQuadGameObject>(groundTrans, shadowRenderPass.getDepthBufferId(), "shadowMapSampler", "shadow", "shadow"));
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

