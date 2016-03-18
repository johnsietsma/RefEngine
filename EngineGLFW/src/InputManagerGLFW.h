#pragma once

#include <input/InputManager.h>

#include <memory>

class GameObject;
struct GLFWwindow;

class InputManagerGLFW : public InputManager
{
public:
    InputManagerGLFW();

    void setWindowCallbacks( GLFWwindow* pWindow );

    void pollEvents() const;
};
