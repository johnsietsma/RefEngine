#pragma once

#include "Input.h"

#include <memory>

class GameObject;

class Component
{
public:
    Component( std::weak_ptr<GameObject> gameObject ) :
        m_gameObject(gameObject)
    {}

    virtual void OnKeyEvent( Input::Key key ) {};

protected:
    std::weak_ptr<GameObject> getGameObject() { return m_gameObject; }

private:
    std::weak_ptr<GameObject> m_gameObject;
};
