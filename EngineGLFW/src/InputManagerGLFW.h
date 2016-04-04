#pragma once

#include <input/InputManager.h>

#include <memory>

class GameObject;
struct GLFWwindow;

class InputManagerGLFW : public InputManager
{
public:
    InputManagerGLFW(std::shared_ptr<InputEventHandler> pInputHandler);

    void setWindowCallbacks( GLFWwindow* pWindow );

    void pollEvents() const;
};
