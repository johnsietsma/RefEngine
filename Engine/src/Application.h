#pragma once

#include <memory>

class Engine;
class InputManager;
class Window;

class Application
{
public:
    Application(std::shared_ptr<Window> pWindow, std::shared_ptr<InputManager> pInputManager);

    virtual float getTime() const = 0;
    virtual void run() = 0;

    bool startup();
    void shutdown();
    
    std::shared_ptr<Window> getWindow() { return m_pWindow; }
    std::shared_ptr<Engine> getEngine() { return m_pEngine; }
    std::shared_ptr<InputManager> getInputManager() { return m_pInputManager; }

protected:
    std::shared_ptr<Window> m_pWindow;
    std::shared_ptr<InputManager> m_pInputManager;
    std::shared_ptr<Engine> m_pEngine;
};
