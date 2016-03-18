#include "ApplicationGLFW.h"

#include "Engine.h"
#include "WindowGLFW.h"
#include "InputManagerGLFW.h"

#include "engine/GameObjectManager.h"
#include <GLFW/glfw3.h>


ApplicationGLFW::ApplicationGLFW(const char* title, int width, int height) :
    Application(std::make_shared<WindowGLFW>("TestBed", 1024, 768), std::make_shared<InputManagerGLFW>())
{
    auto pInputManagerGLFW = static_cast<InputManagerGLFW*>(m_pInputManager.get());
    auto pWindowGLFW = static_cast<WindowGLFW*>(m_pWindow.get());

    pInputManagerGLFW->regsiterEventHandler(m_pEngine->getGameObjectManager().lock());
    pInputManagerGLFW->setWindowCallbacks(pWindowGLFW->getWindow());
}

float ApplicationGLFW::getTime() const
{
    return (float)glfwGetTime();
}


void ApplicationGLFW::run() {

    double prevTime = getTime();
    double currTime = 0;

    while (!m_pWindow->shouldClose() && !m_pInputManager->isKeyDown(Input::Key::Escape)) {
        currTime = getTime();
        float deltaTime = (float)(currTime - prevTime);

        m_pEngine->update(deltaTime);

        m_pInputManager->pollEvents();
        m_pEngine->draw();
        m_pWindow->swapBuffers();

        prevTime = currTime;
    }
}
