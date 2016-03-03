#include "Engine.h"

#include "Engine/Camera.h"
#include "Engine/GameObject.h"
#include "Engine/Helpers.h"
#include "Engine/InputManager.h"
#include "Engine/Light.h"
#include "Engine/RenderPass.h"

#include "Gizmos.h"
#include "gl_core_4_4.h"
#include "Window.h"

#include <glm/glm.hpp>
#include <glm/geometric.hpp>
#include <GLFW/glfw3.h>

#include <assert.h>
#include <iostream>

using glm::vec3;
using glm::vec4;

Engine::Engine( const char* pWindowName ) :
    m_pWindow(std::make_shared<Window>(pWindowName, 1024, 768)),
    m_shouldDrawGrid(true)
{
    if( !m_pWindow->isValid() ) return;



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
    glm::ivec2 size(1024, 768);
    m_pMainCamera = std::make_shared<Camera>(camTransform, glm::radians(45.f), size.x/(float)size.y, 0.1f, 1000.f);
    m_cameras.push_back( m_pMainCamera );

    // Setup a default render pass that uses the main camera and renders to the backbuffer
    m_renderPasses.emplace_back(m_pMainCamera, glm::vec3(0.25f, 0.25f, 0.25f), size);

    // Add a single, hard-coded light
    Transform lightTransform( glm::vec3(0), glm::angleAxis(glm::radians(185.f), glm::vec3(0,1,0 )) );
    m_pLight = std::make_shared<Light>( lightTransform );
}


Engine::~Engine()
{
}



bool Engine::startup()
{
    if( !m_pWindow->isValid() ) return false;

    m_pInputManager = std::make_shared<InputManager>(m_pWindow->getWindow(), shared_from_this());

    // start the gizmo system that can draw basic shapes
    Gizmos::create();

    for (auto& gameObject : m_gameObjects)
    {
        if (!gameObject->create())
            return false;
    }

    return true;
}

void Engine::shutdown()
{

    for (auto& gameObject : m_gameObjects)
    {
        gameObject->destroy();
    }
    m_gameObjects.clear();

    for (auto& renderPass : m_renderPasses)
    {
        renderPass.destroy();
    }
    m_renderPasses.clear();

    // delete our camera and cleanup gizmos
    Gizmos::destroy();

}

void Engine::run() {

    double prevTime = glfwGetTime();
    double currTime = 0;

    while ((currTime = glfwGetTime()) && update((float)(currTime - prevTime))) {

        glfwPollEvents();
        draw();
        glfwSwapBuffers(m_pWindow->getWindow());

        prevTime = currTime;
    }
}

bool Engine::update(float deltaTime) {

    // close the application if the window closes
    if (glfwWindowShouldClose(m_pWindow->getWindow()) ||
        glfwGetKey(m_pWindow->getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
        return false;

    // update the camera's movement
    m_pMainCamera->update(deltaTime);

    //TODO: Temp, make light's into components
    m_pLight->getTransform().rotate(40*deltaTime);

    for (auto& gameObject : m_gameObjects)
    {
        gameObject->update(deltaTime);
    }

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
        glm::ivec2 fboSize = renderPass.getSize();
        glBindFramebuffer(GL_FRAMEBUFFER, renderPass.getId()); // fboId may be 0
        glViewport(0, 0, fboSize.x, fboSize.y);

        // clear the screen for this frame
        glm::vec3 clearColor = renderPass.getClearColor();
        glClearColor(clearColor.r, clearColor.g, clearColor.b, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        for (auto& gameObject : m_gameObjects)
        {
            auto pCameraWeakPtr = renderPass.getCamera();
            auto pCamera = pCameraWeakPtr.lock();
            if( pCamera!=nullptr ) {
                gameObject->draw(*pCamera, *m_pLight);
            }
            else {
                std::cerr << "Missing for a render pass." << std::endl;
            }
        }
    }

    if( m_shouldDrawGrid ) {
        // display the 3D gizmos
        Gizmos::draw(m_pMainCamera->getProjectionView());
    }
}

void Engine::addRenderPass(const RenderPass& renderPass)
{
    assert(m_renderPasses.size() > 0);
    // Leave the default RenderPass at the end, insert at 1 before the end
    m_renderPasses.insert(m_renderPasses.begin() + m_renderPasses.size() - 1, renderPass);
}
