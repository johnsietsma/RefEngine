#include <Application.h>

#include <glm/vec2.hpp>

#include <QTime>

class Window;
class InputManager;

class ApplicationQT : public Application
{
public:
    ApplicationQT(std::shared_ptr<Window> pWindow, std::shared_ptr<InputManager> pInputManager);

    float getTime() const override;

    void run() override;

private:
    QTime m_timer;

};
