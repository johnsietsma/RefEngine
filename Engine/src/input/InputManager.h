#pragma once

#include "Input.h"

#include <glm/vec2.hpp>

#include <memory>
#include <vector>

class InputEventHandler;

class InputManager
{
public:
    InputManager(std::shared_ptr<InputEventHandler> pInputHandler) :
        m_currentKeys{ false },
        m_currentMouseButtons{ false }
    {
        regsiterEventHandler(pInputHandler);
    }

    virtual ~InputManager() = default;

    bool isKeyDown( Input::Key key ) const {
        return m_currentKeys[(int)key];
    }
    bool isMouseButtonDown( Input::MouseButton buton ) const { return m_currentMouseButtons[(int)buton]; }
    glm::vec2 getMousePosition() const { return m_mousePos; }

    void onKeyEvent(Input::Key key, Input::Action action);
    void onMouseButton(Input::MouseButton button, Input::Action action);
    void onMouseMove(float xPos, float yPos);

private:
    // To recieve input event callbacks
    void regsiterEventHandler(std::shared_ptr<InputEventHandler> pInputHandler) { m_eventHanders.push_back(pInputHandler); }

    std::vector< std::shared_ptr<InputEventHandler> > m_eventHanders;
    glm::vec2 m_mousePos;
    bool m_currentKeys[(int)Input::Key::Count];
    bool m_currentMouseButtons[(int)Input::MouseButton::Count];
};
