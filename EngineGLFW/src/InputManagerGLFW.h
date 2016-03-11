#pragma once

#include <input/InputManager.h>

#include <memory>

class GameObject;
struct GLFWwindow;

class InputManagerGLFW : public InputManager
{
public:
    InputManagerGLFW( GLFWwindow* pWindow );

    void pollEvents() const;

private:
};
