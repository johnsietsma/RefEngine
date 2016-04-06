#include "Engine.h"

#include "gameObjects/FlyCameraGameObject.h"

#include "engine/Camera.h"
#include "engine/GameObject.h"
#include "engine/GameObjectManager.h"
#include "engine/Helpers.h"
#include "engine/Light.h"
#include "engine/Window.h"

#include "graphics/RenderPass.h"

#include "input/InputManager.h"

#include "Gizmos.h"
#include "graphics/RefOpenGL.h"

#include <glm/glm.hpp>
#include <glm/geometric.hpp>

#include <assert.h>
#include <iostream>

using glm::vec3;
using glm::vec4;

Engine::Engine() :
    m_pGameObjectManager(std::make_shared<GameObjectManager>()),
    m_pLight(nullptr),
    m_shouldDrawGrid(true)
{

}


Engine::~Engine()
{
}


bool Engine::startup()
{
    glClearColor(0.25f, 0.25f, 0.25f, 1);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

#ifdef _DEBUG
    TurnOnOpenGLDebugLogging();
#endif


    // start the gizmo system that can draw basic shapes
    Gizmos::create();

    m_pGameObjectManager->create();

    return true;
}

void Engine::shutdown()
{
    m_pGameObjectManager->destroy();

    for (auto& renderPass : m_renderPasses)
    {
        renderPass.destroy();
    }
    m_renderPasses.clear();

    // delete our camera and cleanup gizmos
    Gizmos::destroy();

}

void Engine::update(float deltaTime) {

    if (m_pLight != nullptr) {
        //TODO: Temp, make light's into components
        glm::quat rot = glm::angleAxis(1 * deltaTime, Transform::WORLD_UP);
        m_pLight->getTransform().rotate(rot);
    }

    m_pGameObjectManager->update(deltaTime);
}

void Engine::draw(GLuint backbufferId)
{
    if (m_cameras.size() == 0) {
        glClearColor(1,0,1,1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        return;
    }

    if( m_shouldDrawGrid ) {
        Gizmos::clear();

        Gizmos::addTransform(glm::mat4());

        // ...for now let's add a grid to the gizmos
        for (int i = 0; i < 21; ++i) {
            Gizmos::addLine(vec3(-10 + i, 0, 10), vec3(-10 + i, 0, -10),
                i == 10 ? vec4(1, 1, 1, 1) : vec4(0, 0, 0, 1));

            Gizmos::addLine(vec3(10, 0, -10 + i), vec3(-10, 0, -10 + i),
                i == 10 ? vec4(1, 1, 1, 1) : vec4(0, 0, 0, 1));
        }
    }


    for (RenderPass& renderPass : m_renderPasses)
    {
        // Get the camera to render from
        auto pCameraWeakPtr = renderPass.getCamera();
        auto pCamera = pCameraWeakPtr.lock();
        if (pCamera == nullptr)
        {
            std::cerr << "Missing camera for a render pass." << std::endl;
            continue;
        }

        glm::ivec2 fboSize = renderPass.getSize();
        GLuint fboId = renderPass.getId();
        fboId = fboId == 0 ? backbufferId : fboId;
        glBindFramebuffer(GL_FRAMEBUFFER, fboId);
        glViewport(0, 0, fboSize.x, fboSize.y);

        // clear the screen for this frame
        glm::vec3 clearColor = renderPass.getClearColor();
        glClearColor(clearColor.r, clearColor.g, clearColor.b, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_pGameObjectManager->draw(*pCamera, m_pLight.get(), renderPass.getProgram(), renderPass.getLayers());
    }

    if( m_shouldDrawGrid ) {
        // display the 3D gizmos
        Gizmos::draw(getMainCamera()->getProjectionViewTransform());
    }
}

void Engine::addRenderPass(const RenderPass& renderPass)
{
    m_renderPasses.push_back(renderPass);
}
