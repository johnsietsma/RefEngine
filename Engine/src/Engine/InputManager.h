#pragma once

#include "Input.h"
#include "Visitor.h"

#include <memory>

class Engine;
class GameObject;
struct GLFWwindow;

using KeyVisitor = Visitor<std::shared_ptr<GameObject>,Input::Key>;

class InputManager
{
public:
    InputManager( GLFWwindow* pWindow, Engine* pEngine );

    KeyVisitor getKeyVisitor() { return m_keyVisitor; }
private:
    KeyVisitor m_keyVisitor;
};
