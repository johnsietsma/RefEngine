#pragma once

#include <input/InputManager.h>

class InputManagerQT : public InputManager
{
public:
    InputManagerQT(std::shared_ptr<InputEventHandler> pInputHandler);
    ~InputManagerQT() = default;
};
