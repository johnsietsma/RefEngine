#include "engine/Component.h"

#include <glm/vec3.hpp>

#include <memory>

class GameObject;
class InputManager;


class FlyComponent : public Component
{
public:
    FlyComponent(std::shared_ptr<GameObject> pGameObject, std::shared_ptr<InputManager> pInputManager ) :
        Component(pGameObject),
        m_speedMultiplier(3),
        m_pInputManager(pInputManager)
    {
    }
    
    void update( float deltaTime ) override;
    
private:
    float m_speedMultiplier;
    std::shared_ptr<InputManager> m_pInputManager;
};
