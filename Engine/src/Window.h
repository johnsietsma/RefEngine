
#include <glm/vec2.hpp>

struct GLFWwindow;

class Window
{
public:
    Window( const char* title, int width, int height );
    ~Window();

    bool isValid() const { return m_pWindow!=nullptr; }

    GLFWwindow* getWindow() const { return m_pWindow; }
    
    // The frame buffer size in pixels.
    // Can be bigger then the screen on Retina displays.
    glm::ivec2 getFramebufferSize() const;
    
    // The window size in pixels
    glm::ivec2 getWindowSize() const;
    float getAspectRatio() const;

private:
    GLFWwindow* m_pWindow;
};
