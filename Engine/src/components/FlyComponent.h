#include "engine/Component.h"

#include <glm/vec3.hpp>

#include <memory>

class GameObject;
class InputManager;


class FlyComponent : public Component
{
    FlyComponent(std::shared_ptr<GameObject> pGameObject, std::shared_ptr<InputManager> pInputManager ) :
        Component(pGameObject),
        m_moveDir(0),
        m_pInputManager(pInputManager)
    {
    }

    void update(float deltaTime) override;

private:
    float m_speed = 1;
    float m_speedMultiplier = 1;
    glm::vec3 m_moveDir;

    std::shared_ptr<InputManager> m_pInputManager;
};