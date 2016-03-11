#include "Engine.h"

#include "components/CameraComponent.h"

#include "engine/Camera.h"
#include "engine/GameObject.h"
#include "engine/GameObjectManager.h"
#include "engine/Helpers.h"
#include "engine/Light.h"
#include "engine/Window.h"

#include "graphics/RenderPass.h"

#include "input/InputManager.h"

#include "Gizmos.h"
#include "gl_core_4_4.h"

#include <glm/glm.hpp>
#include <glm/geometric.hpp>

#include <assert.h>
#include <iostream>

using glm::vec3;
using glm::vec4;

Engine::Engine(std::shared_ptr<Window> pWindow, std::shared_ptr<InputManager> pInputManager) :
    m_pWindow(pWindow),
    m_pInputManager(pInputManager),
    m_pGameObjectManager(std::make_shared<GameObjectManager>()),
    m_shouldDrawGrid(true)
{
    if( !m_pWindow->isValid() ) return;

    pInputManager->regsiterEventHandler(m_pGameObjectManager);

    auto major = ogl_GetMajorVersion();
    auto minor = ogl_GetMinorVersion();
    std::cout << "GL: " << major << "." << minor << std::endl;

    glClearColor(0.25f, 0.25f, 0.25f, 1);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

#ifdef _DEBUG
    TurnOnOpenGLDebugLogging();
#endif

    // create a default camera
    Transform camTransform(vec3(2, 6, 13), vec3(0));
    glm::ivec2 size = m_pWindow->getFramebufferSize();
    m_pMainCamera = std::make_shared<Camera>(camTransform, glm::radians(45.f), size.x/(float)size.y, 0.1f, 1000.f);
    m_cameras.push_back( m_pMainCamera );

    // Setup a default render pass that uses the main camera and renders to the backbuffer
    m_renderPasses.emplace_back(m_pMainCamera, glm::vec3(0.25f, 0.25f, 0.25f), size);

    // Add a single, hard-coded light
    Transform lightTransform( glm::vec3(1,1,0), glm::vec3(0) );
    m_pLight = std::make_shared<Light>( lightTransform );
}


Engine::~Engine()
{
}



bool Engine::startup()
{
    if( !m_pWindow->isValid() ) return false;

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

void Engine::run() {

    double prevTime = m_pWindow->getTime();
    double currTime = 0;

    while ((currTime = m_pWindow->getTime()) && update((float)(currTime - prevTime))) {

        m_pInputManager->pollEvents();
        draw();
        m_pWindow->swapBuffers();

        prevTime = currTime;
    }
}

bool Engine::update(float deltaTime) {
    //TODO: Temp, make light's into components
    glm::quat rot = glm::angleAxis(1 * deltaTime, Transform::WORLD_UP);
    m_pLight->getTransform().rotate(rot);

    m_pGameObjectManager->update(deltaTime);

    // return true, else the application closes
    return true;
}

void Engine::draw()
{
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
        auto pCameraWeakPtr = renderPass.getCameraComponent();
        auto pCamera = pCameraWeakPtr.lock();
        if (pCamera == nullptr)
        {
            std::cerr << "Missing camera for a render pass." << std::endl;
            continue;
        }

        glm::ivec2 fboSize = renderPass.getSize();
        glBindFramebuffer(GL_FRAMEBUFFER, renderPass.getId()); // fboId may be 0
        glViewport(0, 0, fboSize.x, fboSize.y);

        // clear the screen for this frame
        glm::vec3 clearColor = renderPass.getClearColor();
        glClearColor(clearColor.r, clearColor.g, clearColor.b, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_pGameObjectManager->draw(*pCamera, *m_pLight, renderPass.getProgram(), renderPass.getLayers());
    }

    if( m_shouldDrawGrid ) {
        // display the 3D gizmos
        Gizmos::draw(m_pMainCamera->getProjectionViewTransform());
    }
}

void Engine::addRenderPass(const RenderPass& renderPass)
{
    assert(m_renderPasses.size() > 0);
    // Leave the default RenderPass at the end, insert at 1 before the end
    m_renderPasses.insert(m_renderPasses.begin() + m_renderPasses.size() - 1, renderPass);
}
