#pragma once

#include <input/InputManager.h>

#include <QObject>

class InputManagerQT : public QObject, public InputManager 
{
    Q_OBJECT

public:
    InputManagerQT(std::shared_ptr<InputEventHandler> pInputHandler);
    ~InputManagerQT() = default;

protected:
    bool eventFilter(QObject *obj, QEvent *event);

private:
    bool handleKeyEvent(QEvent *event, Input::Action action);
    bool handleMouseEvent(QEvent *event, Input::Action action);
};
