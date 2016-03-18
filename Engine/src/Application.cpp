#include "Application.h"

#include "Engine.h"
#include "engine/GameObjectManager.h"
#include "engine/Window.h"
#include "input/InputManager.h"

Application::Application(std::shared_ptr<Window> pWindow, std::shared_ptr<InputManager> pInputManager) :
    m_pWindow(pWindow),
    m_pInputManager(pInputManager),
    m_pEngine(std::make_shared<Engine>())
{
    if (!m_pWindow->isValid()) return;

    std::shared_ptr<GameObjectManager> pGameObjectManager = m_pEngine->getGameObjectManager().lock();
    assert(pGameObjectManager != nullptr);
    m_pInputManager->regsiterEventHandler( pGameObjectManager );
}


bool Application::startup()
{
    return m_pEngine->startup();
}

void Application::shutdown()
{
    return m_pEngine->shutdown();
}


