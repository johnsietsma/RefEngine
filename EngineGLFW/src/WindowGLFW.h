#include <engine/Window.h>

#include <glm/vec2.hpp>

struct GLFWwindow;

class WindowGLFW : public Window
{
public:
    WindowGLFW( const char* title, int width, int height );
    ~WindowGLFW();

    GLFWwindow* getWindow() const { return m_pWindow; }

    bool isValid() const override { return m_pWindow!=nullptr; }
    bool shouldClose() const override;

    float getTime() const override;
    void swapBuffers() const override;

    // The frame buffer size in pixels.
    // Can be bigger then the screen on Retina displays.
    glm::ivec2 getFramebufferSize() const override;
    
    // The window size in pixels
    glm::ivec2 getWindowSize() const override;
    float getAspectRatio() const override;

private:
    GLFWwindow* m_pWindow;
};
