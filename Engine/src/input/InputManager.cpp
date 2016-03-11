#include "InputManager.h"

#include "Engine.h"
#include "engine/GameObject.h"
#include "engine/Component.h"
#include "input/InputEventHandler.h"

void InputManager::onKeyEvent(Input::Key key, Input::Action action)
{
    if (action == Input::Action::Press)
    {
        m_currentKeys[(int)key] = true;
    }
    else if (action == Input::Action::Release)
    {
        m_currentKeys[(int)key] = false;
    }

    for (auto pEventHandler : m_eventHanders)
    {
        pEventHandler->onKeyEvent(key, action);
    }
}

void InputManager::onMouseButton(Input::MouseButton button, Input::Action action)
{
    if (action == Input::Action::Press)
    {
        m_currentMouseButtons[(int)button] = true;
    }
    else if(action == Input::Action::Release)
    {
        m_currentMouseButtons[(int)button] = false;
    }

    for (auto pEventHandler : m_eventHanders)
    {
        pEventHandler->onMouseButton(button, action);
    }
}


void InputManager::onMouseMove(float xPos, float yPos)
{
    m_mousePos = glm::ivec2(xPos, yPos);
}
