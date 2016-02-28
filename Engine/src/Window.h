
#include <glm/vec2.hpp>

struct GLFWwindow;

class Window
{
public:
    Window( const char* title, int width, int height );
    ~Window();

    bool isValid() const { return m_pWindow!=nullptr; }

    GLFWwindow* getWindow() const { return m_pWindow; }
    float getAspectRatio() const;

private:
    GLFWwindow* m_pWindow;

};
