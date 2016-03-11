#include "FlyComponent.h"

#include "engine/GameObject.h"
#include "input/Input.h"
#include "input/InputManager.h"

#include <glm/vec2.hpp>

void  FlyComponent::update(float deltaTime)
{
    m_speedMultiplier = m_pInputManager->isKeyDown(Input::Key::LeftShift) ? 2.f : 1.f;

    auto pGameObject = getGameObject().lock();
    if (pGameObject == nullptr) return;
    Transform transform = pGameObject->getTransform();

    // translate
    if( m_pInputManager->isKeyDown(Input::Key::W) ) m_moveDir =  transform.getForward();
    if( m_pInputManager->isKeyDown(Input::Key::S) ) m_moveDir = -transform.getForward();
    if( m_pInputManager->isKeyDown(Input::Key::D) ) m_moveDir =  transform.getRight();
    if( m_pInputManager->isKeyDown(Input::Key::A) ) m_moveDir = -transform.getRight();
    if( m_pInputManager->isKeyDown(Input::Key::Q) ) m_moveDir =  transform.getUp();
    if( m_pInputManager->isKeyDown(Input::Key::E) ) m_moveDir = -transform.getUp();


    // check for rotation
    static bool sbMouseButtonDown = false;
    if (m_pInputManager->isMouseButtonDown(Input::MouseButton::Left) ||  m_pInputManager->isKeyDown(Input::Key::Space))
    {
        static glm::ivec2 prevPos(0);

        if (sbMouseButtonDown == false)
        {
            sbMouseButtonDown = true;
            prevPos = m_pInputManager->getMousePosition();
        }

        glm::ivec2 pos(0);
        pos = m_pInputManager->getMousePosition();

        glm::ivec2 deltaPos = pos - prevPos;

        prevPos = pos;

        glm::mat4 mMat;

        // pitch
        if (deltaPos.y != 0)
        {
            glm::quat rotation = glm::angleAxis((float)-deltaPos.y / 150.0f, transform.getRight());
            transform.rotate(rotation);
        }

        // yaw
        if (deltaPos.x != 0)
        {
            glm::quat rotation = glm::angleAxis((float)-deltaPos.x / 150.0f, glm::vec3(0,1,0));
            transform.rotate(rotation);
        }
    }
    else
    {
        sbMouseButtonDown = false;
    }

    pGameObject->setTransform(transform);

}
