#include <Application.h>

#include <glm/vec2.hpp>

class Window;
class InputManager;

class ApplicationGLFW : public Application
{
public:
    ApplicationGLFW(const char* title, int width, int height);

    float getTime() const override;

    void run() override;

};
