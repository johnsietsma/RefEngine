#pragma once

#include "input/Input.h"

#include <glm/vec2.hpp>

#include <memory>

class GameObject;

class Component
{
public:
    Component( std::weak_ptr<GameObject> gameObject ) :
        m_gameObject(gameObject)
    {}

    glm::vec2 getMousePos() const { return m_mousePos;  }

    virtual void onKeyEvent( Input::Key key, Input::Action action ) {};
    virtual void onMouseButton(Input::MouseButton button, Input::Action action) {};

    virtual void update( float deltaTime ) {};

protected:
    std::weak_ptr<GameObject> getGameObject() { return m_gameObject; }
    const std::weak_ptr<GameObject> getGameObject() const { return m_gameObject; }

private:
    std::weak_ptr<GameObject> m_gameObject;
    glm::vec2 m_mousePos;
};
