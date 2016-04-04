#include "InputManagerQT.h"

#include <unordered_map>

#include <QKeyEvent>

static std::unordered_map<int,Input::Key> keyCodeMap;

void makeKeyCodeMap()
{
    keyCodeMap[Qt::Key_Backspace] = Input::Key::Backspace;
    keyCodeMap[Qt::Key_Tab] = Input::Key::Tab;
    keyCodeMap[Qt::Key_Enter] = Input::Key::Enter;
    keyCodeMap[Qt::Key_Pause] = Input::Key::Pause;
    keyCodeMap[Qt::Key_CapsLock] = Input::Key::CapsLock;
    keyCodeMap[Qt::Key_Escape] = Input::Key::Escape;
    keyCodeMap[Qt::Key_Space] = Input::Key::Space;
    keyCodeMap[Qt::Key_PageUp] = Input::Key::PageUp;
    keyCodeMap[Qt::Key_PageDown] = Input::Key::PageDown;
    keyCodeMap[Qt::Key_End] = Input::Key::End;
    keyCodeMap[Qt::Key_Home] = Input::Key::Home;
    keyCodeMap[Qt::Key_Left] = Input::Key::Left;
    keyCodeMap[Qt::Key_Up] = Input::Key::Up;
    keyCodeMap[Qt::Key_Right] = Input::Key::Right;
    keyCodeMap[Qt::Key_Down] = Input::Key::Down;
    keyCodeMap[Qt::Key_Print] = Input::Key::PrintScreen;
    keyCodeMap[Qt::Key_Insert] = Input::Key::Insert;
    keyCodeMap[Qt::Key_Delete] = Input::Key::Delete;
    keyCodeMap[Qt::Key_0] = Input::Key::Zero;
    keyCodeMap[Qt::Key_1] = Input::Key::One;
    keyCodeMap[Qt::Key_2] = Input::Key::Two;
    keyCodeMap[Qt::Key_3] = Input::Key::Three;
    keyCodeMap[Qt::Key_4] = Input::Key::Four;
    keyCodeMap[Qt::Key_5] = Input::Key::Five;
    keyCodeMap[Qt::Key_6] = Input::Key::Six;
    keyCodeMap[Qt::Key_7] = Input::Key::Seven;
    keyCodeMap[Qt::Key_8] = Input::Key::Eight;
    keyCodeMap[Qt::Key_9] = Input::Key::Nine;
    keyCodeMap[Qt::Key_A] = Input::Key::A;
    keyCodeMap[Qt::Key_B] = Input::Key::B;
    keyCodeMap[Qt::Key_C] = Input::Key::C;
    keyCodeMap[Qt::Key_D] = Input::Key::D;
    keyCodeMap[Qt::Key_E] = Input::Key::E;
    keyCodeMap[Qt::Key_F] = Input::Key::F;
    keyCodeMap[Qt::Key_G] = Input::Key::G;
    keyCodeMap[Qt::Key_H] = Input::Key::H;
    keyCodeMap[Qt::Key_I] = Input::Key::I;
    keyCodeMap[Qt::Key_J] = Input::Key::J;
    keyCodeMap[Qt::Key_K] = Input::Key::K;
    keyCodeMap[Qt::Key_L] = Input::Key::L;
    keyCodeMap[Qt::Key_M] = Input::Key::M;
    keyCodeMap[Qt::Key_N] = Input::Key::N;
    keyCodeMap[Qt::Key_O] = Input::Key::O;
    keyCodeMap[Qt::Key_P] = Input::Key::P;
    keyCodeMap[Qt::Key_Q] = Input::Key::Q;
    keyCodeMap[Qt::Key_R] = Input::Key::R;
    keyCodeMap[Qt::Key_S] = Input::Key::S;
    keyCodeMap[Qt::Key_T] = Input::Key::T;
    keyCodeMap[Qt::Key_U] = Input::Key::U;
    keyCodeMap[Qt::Key_V] = Input::Key::V;
    keyCodeMap[Qt::Key_W] = Input::Key::W;
    keyCodeMap[Qt::Key_X] = Input::Key::X;
    keyCodeMap[Qt::Key_Y] = Input::Key::Y;
    keyCodeMap[Qt::Key_Z] = Input::Key::Z;
    keyCodeMap[Qt::Key_0] = Input::Key::NumPad0;
    keyCodeMap[Qt::Key_1] = Input::Key::NumPad1;
    keyCodeMap[Qt::Key_2] = Input::Key::NumPad2;
    keyCodeMap[Qt::Key_3] = Input::Key::NumPad3;
    keyCodeMap[Qt::Key_4] = Input::Key::NumPad4;
    keyCodeMap[Qt::Key_5] = Input::Key::NumPad5;
    keyCodeMap[Qt::Key_6] = Input::Key::NumPad6;
    keyCodeMap[Qt::Key_7] = Input::Key::NumPad7;
    keyCodeMap[Qt::Key_8] = Input::Key::NumPad8;
    keyCodeMap[Qt::Key_9] = Input::Key::NumPad9;
    keyCodeMap[Qt::Key_Asterisk] = Input::Key::Multiply;
    keyCodeMap[Qt::Key_Plus] = Input::Key::Add;
    keyCodeMap[Qt::Key_Minus] = Input::Key::Subtract;
    keyCodeMap[Qt::Key_Period] = Input::Key::Decimal;
    keyCodeMap[Qt::Key_Slash] = Input::Key::Divide;
    keyCodeMap[Qt::Key_F1] = Input::Key::F1;
    keyCodeMap[Qt::Key_F2] = Input::Key::F2;
    keyCodeMap[Qt::Key_F3] = Input::Key::F3;
    keyCodeMap[Qt::Key_F4] = Input::Key::F4;
    keyCodeMap[Qt::Key_F5] = Input::Key::F5;
    keyCodeMap[Qt::Key_F6] = Input::Key::F6;
    keyCodeMap[Qt::Key_F7] = Input::Key::F7;
    keyCodeMap[Qt::Key_F8] = Input::Key::F8;
    keyCodeMap[Qt::Key_F9] = Input::Key::F9;
    keyCodeMap[Qt::Key_F10] = Input::Key::F10;
    keyCodeMap[Qt::Key_F11] = Input::Key::F11;
    keyCodeMap[Qt::Key_F12] = Input::Key::F12;
    keyCodeMap[Qt::Key_F13] = Input::Key::F13;
    keyCodeMap[Qt::Key_F14] = Input::Key::F14;
    keyCodeMap[Qt::Key_F15] = Input::Key::F15;
    keyCodeMap[Qt::Key_F16] = Input::Key::F16;
    keyCodeMap[Qt::Key_F17] = Input::Key::F17;
    keyCodeMap[Qt::Key_F18] = Input::Key::F18;
    keyCodeMap[Qt::Key_F19] = Input::Key::F19;
    keyCodeMap[Qt::Key_F20] = Input::Key::F20;
    keyCodeMap[Qt::Key_F21] = Input::Key::F21;
    keyCodeMap[Qt::Key_F22] = Input::Key::F22;
    keyCodeMap[Qt::Key_F23] = Input::Key::F23;
    keyCodeMap[Qt::Key_F24] = Input::Key::F24;
    keyCodeMap[Qt::Key_NumLock] = Input::Key::Numlock;
    keyCodeMap[Qt::Key_ScrollLock] = Input::Key::ScrollLock;
    keyCodeMap[Qt::Key_Shift] = Input::Key::LeftShift;
    keyCodeMap[Qt::Key_Shift] = Input::Key::RightShift;
    keyCodeMap[Qt::Key_Control] = Input::Key::LeftControl;
    keyCodeMap[Qt::Key_Control] = Input::Key::RightContol;
    keyCodeMap[Qt::Key_Menu] = Input::Key::Menu;
}

InputManagerQT::InputManagerQT(std::shared_ptr<InputEventHandler> pInputHandler) :
    InputManager(pInputHandler)
{
    makeKeyCodeMap();
}


