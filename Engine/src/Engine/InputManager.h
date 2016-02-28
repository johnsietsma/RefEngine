#pragma once

#include "Input.h"

#include <memory>

class Engine;
class GameObject;
struct GLFWwindow;

class InputManager
{
public:
    InputManager( GLFWwindow* pWindow, std::shared_ptr<Engine> pEngine );

    void onKeyEvent( Input::Key key );

private:
    std::weak_ptr<Engine> m_pEngine;
};
