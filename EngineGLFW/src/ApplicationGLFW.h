#include <memory>

class Engine;
class InputManager;
class InputManagerGLFW;
class Window;
class WindowGLFW;


class ApplicationGLFW
{
public:
    ApplicationGLFW(const char* title, int width, int height);

    float getTime() const;
    void run();

    bool startup();
    void shutdown();

    std::shared_ptr<Engine> getEngine() { return m_pEngine; }
    std::shared_ptr<InputManager> getInputManager() { return m_pInputManager; }
    std::shared_ptr<Window> getWindow() { return m_pWindow; }

protected:
    std::shared_ptr<Engine> m_pEngine;
    std::shared_ptr<InputManager> m_pInputManager;
    std::shared_ptr<Window> m_pWindow;

};
