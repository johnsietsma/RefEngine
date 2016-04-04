#include "ApplicationGLFW.h"

#include "Engine.h"
#include "WindowGLFW.h"
#include "InputManagerGLFW.h"

#include "engine/GameObjectManager.h"
#include <GLFW/glfw3.h>


ApplicationGLFW::ApplicationGLFW(const char* title, int width, int height)
{
    m_pEngine = std::make_shared<Engine>();

    std::shared_ptr<InputManagerGLFW> pInputManagerGLFW = std::make_shared<InputManagerGLFW>(m_pEngine->getGameObjectManager().lock());
    m_pInputManager = pInputManagerGLFW;

    std::shared_ptr<WindowGLFW> pWindowGLFW = std::make_shared<WindowGLFW>(title, width, height);
    m_pWindow = pWindowGLFW;

    pInputManagerGLFW->setWindowCallbacks(pWindowGLFW->getWindow());
}

float ApplicationGLFW::getTime() const
{
    return (float)glfwGetTime();
}

bool ApplicationGLFW::startup()
{
    return m_pEngine->startup();
}

void ApplicationGLFW::shutdown()
{
    return m_pEngine->shutdown();
}


void ApplicationGLFW::run() {

    double prevTime = getTime();
    double currTime = 0;

    auto pWindowGLFW = static_cast<WindowGLFW*>(m_pWindow.get());
    auto pInputManagerGLFW = static_cast<InputManagerGLFW*>(m_pInputManager.get());

    while (!pWindowGLFW->shouldClose() && !m_pInputManager->isKeyDown(Input::Key::Escape)) {
        currTime = getTime();
        float deltaTime = (float)(currTime - prevTime);

        m_pEngine->update(deltaTime);
        m_pEngine->draw();

        pInputManagerGLFW->pollEvents();
        pWindowGLFW->swapBuffers();

        prevTime = currTime;
    }
}
