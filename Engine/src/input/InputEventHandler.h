#pragma once

#include "input/Input.h"

class InputEventHandler
{
public:
    virtual void onKeyEvent(Input::Key key, Input::Action action) = 0;
    virtual void onMouseButton(Input::MouseButton button, Input::Action action) = 0;
};
