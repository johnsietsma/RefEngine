#include "ApplicationQT.h"

#include "Engine.h"
#include "engine/Window.h"
#include "input/InputManager.h"



ApplicationQT::ApplicationQT(std::shared_ptr<Window> pWindow, std::shared_ptr<InputManager> pInputManager) :
    Application( pWindow, pInputManager )
{
    m_timer.start();
}

float ApplicationQT::getTime() const
{
    return m_timer.elapsed();
}


void ApplicationQT::run() {

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
