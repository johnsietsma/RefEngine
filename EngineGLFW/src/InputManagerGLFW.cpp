#include "InputManagerGLFW.h"

#include <GLFW/glfw3.h>

#include <unordered_map>

static std::unordered_map<GLFWwindow*, InputManager*> windowManagerMap;
static std::unordered_map<int,Input::Key> keyCodeMap;



Input::Key getKey( int keyCode )
{
    auto keyItr = keyCodeMap.find(keyCode);
    if( keyItr == keyCodeMap.end() ) return Input::Key::None;
    return keyItr->second;
}

Input::MouseButton getMouseButton( int mouseButonCode )
{
    switch (mouseButonCode)
    {
    case GLFW_MOUSE_BUTTON_LEFT: return Input::MouseButton::Left;
    case GLFW_MOUSE_BUTTON_MIDDLE: return Input::MouseButton::Middle;
    case GLFW_MOUSE_BUTTON_RIGHT: return Input::MouseButton::Right;
    default: assert(false);
    }
    return Input::MouseButton::None;
}

Input::Action getAction( int actionCode )
{
    switch (actionCode)
    {
    case GLFW_PRESS: return Input::Action::Press;
    case GLFW_RELEASE: return Input::Action::Release;
    case GLFW_REPEAT: break; 
    default: break; 
    }
    return Input::Action::None;
}


static void key_callback(GLFWwindow* pWindow, int keyCode, int scancode, int actionCode, int mods)
{
    InputManager* pInputManager = windowManagerMap[pWindow];
    if( pInputManager==nullptr ) return;

    Input::Key key = getKey( keyCode );
    Input::Action action = getAction(actionCode);

    if( key==Input::Key::None ) return;

    pInputManager->onKeyEvent( key, action );
}

static void mouse_button_callback(GLFWwindow* pWindow, int buttonCode, int actionCode, int mods)
{
    InputManager* pInputManager = windowManagerMap[pWindow];
    if (pInputManager == nullptr) return;

    Input::MouseButton button = getMouseButton(buttonCode);
    Input::Action action = getAction(actionCode);

    pInputManager->onMouseButton(button, action);
}

static void cursor_position_callback(GLFWwindow* pWindow, double xPos, double yPos)
{
    InputManager* pInputManager = windowManagerMap[pWindow];
    if (pInputManager == nullptr) return;

    pInputManager->onMouseMove((float)xPos, (float)yPos);
}


void makeKeyCodeMap()
{
    keyCodeMap[GLFW_KEY_BACKSPACE] = Input::Key::Backspace;
    keyCodeMap[GLFW_KEY_TAB] = Input::Key::Tab;
    keyCodeMap[GLFW_KEY_ENTER] = Input::Key::Enter;
    keyCodeMap[GLFW_KEY_PAUSE] = Input::Key::Pause;
    keyCodeMap[GLFW_KEY_CAPS_LOCK] = Input::Key::CapsLock;
    keyCodeMap[GLFW_KEY_ESCAPE] = Input::Key::Escape;
    keyCodeMap[GLFW_KEY_SPACE] = Input::Key::Space;
    keyCodeMap[GLFW_KEY_PAGE_UP] = Input::Key::PageUp;
    keyCodeMap[GLFW_KEY_PAGE_DOWN] = Input::Key::PageDown;
    keyCodeMap[GLFW_KEY_END] = Input::Key::End;
    keyCodeMap[GLFW_KEY_HOME] = Input::Key::Home;
    keyCodeMap[GLFW_KEY_LEFT] = Input::Key::Left;
    keyCodeMap[GLFW_KEY_UP] = Input::Key::Up;
    keyCodeMap[GLFW_KEY_RIGHT] = Input::Key::Right;
    keyCodeMap[GLFW_KEY_DOWN] = Input::Key::Down;
    keyCodeMap[GLFW_KEY_PRINT_SCREEN] = Input::Key::PrintScreen;
    keyCodeMap[GLFW_KEY_INSERT] = Input::Key::Insert;
    keyCodeMap[GLFW_KEY_DELETE] = Input::Key::Delete;
    keyCodeMap[GLFW_KEY_0] = Input::Key::Zero;
    keyCodeMap[GLFW_KEY_1] = Input::Key::One;
    keyCodeMap[GLFW_KEY_2] = Input::Key::Two;
    keyCodeMap[GLFW_KEY_3] = Input::Key::Three;
    keyCodeMap[GLFW_KEY_4] = Input::Key::Four;
    keyCodeMap[GLFW_KEY_5] = Input::Key::Five;
    keyCodeMap[GLFW_KEY_6] = Input::Key::Six;
    keyCodeMap[GLFW_KEY_7] = Input::Key::Seven;
    keyCodeMap[GLFW_KEY_8] = Input::Key::Eight;
    keyCodeMap[GLFW_KEY_9] = Input::Key::Nine;
    keyCodeMap[GLFW_KEY_A] = Input::Key::A;
    keyCodeMap[GLFW_KEY_B] = Input::Key::B;
    keyCodeMap[GLFW_KEY_C] = Input::Key::C;
    keyCodeMap[GLFW_KEY_D] = Input::Key::D;
    keyCodeMap[GLFW_KEY_E] = Input::Key::E;
    keyCodeMap[GLFW_KEY_F] = Input::Key::F;
    keyCodeMap[GLFW_KEY_G] = Input::Key::G;
    keyCodeMap[GLFW_KEY_H] = Input::Key::H;
    keyCodeMap[GLFW_KEY_I] = Input::Key::I;
    keyCodeMap[GLFW_KEY_J] = Input::Key::J;
    keyCodeMap[GLFW_KEY_K] = Input::Key::K;
    keyCodeMap[GLFW_KEY_L] = Input::Key::L;
    keyCodeMap[GLFW_KEY_M] = Input::Key::M;
    keyCodeMap[GLFW_KEY_N] = Input::Key::N;
    keyCodeMap[GLFW_KEY_O] = Input::Key::O;
    keyCodeMap[GLFW_KEY_P] = Input::Key::P;
    keyCodeMap[GLFW_KEY_Q] = Input::Key::Q;
    keyCodeMap[GLFW_KEY_R] = Input::Key::R;
    keyCodeMap[GLFW_KEY_S] = Input::Key::S;
    keyCodeMap[GLFW_KEY_T] = Input::Key::T;
    keyCodeMap[GLFW_KEY_U] = Input::Key::U;
    keyCodeMap[GLFW_KEY_V] = Input::Key::V;
    keyCodeMap[GLFW_KEY_W] = Input::Key::W;
    keyCodeMap[GLFW_KEY_X] = Input::Key::X;
    keyCodeMap[GLFW_KEY_Y] = Input::Key::Y;
    keyCodeMap[GLFW_KEY_Z] = Input::Key::Z;
    keyCodeMap[GLFW_KEY_KP_0] = Input::Key::NumPad0;
    keyCodeMap[GLFW_KEY_KP_1] = Input::Key::NumPad1;
    keyCodeMap[GLFW_KEY_KP_2] = Input::Key::NumPad2;
    keyCodeMap[GLFW_KEY_KP_3] = Input::Key::NumPad3;
    keyCodeMap[GLFW_KEY_KP_4] = Input::Key::NumPad4;
    keyCodeMap[GLFW_KEY_KP_5] = Input::Key::NumPad5;
    keyCodeMap[GLFW_KEY_KP_6] = Input::Key::NumPad6;
    keyCodeMap[GLFW_KEY_KP_7] = Input::Key::NumPad7;
    keyCodeMap[GLFW_KEY_KP_8] = Input::Key::NumPad8;
    keyCodeMap[GLFW_KEY_KP_9] = Input::Key::NumPad9;
    keyCodeMap[GLFW_KEY_KP_MULTIPLY] = Input::Key::Multiply;
    keyCodeMap[GLFW_KEY_KP_ADD] = Input::Key::Add;
    keyCodeMap[GLFW_KEY_KP_SUBTRACT] = Input::Key::Subtract;
    keyCodeMap[GLFW_KEY_KP_DECIMAL] = Input::Key::Decimal;
    keyCodeMap[GLFW_KEY_KP_DIVIDE] = Input::Key::Divide;
    keyCodeMap[GLFW_KEY_F1] = Input::Key::F1;
    keyCodeMap[GLFW_KEY_F2] = Input::Key::F2;
    keyCodeMap[GLFW_KEY_F3] = Input::Key::F3;
    keyCodeMap[GLFW_KEY_F4] = Input::Key::F4;
    keyCodeMap[GLFW_KEY_F5] = Input::Key::F5;
    keyCodeMap[GLFW_KEY_F6] = Input::Key::F6;
    keyCodeMap[GLFW_KEY_F7] = Input::Key::F7;
    keyCodeMap[GLFW_KEY_F8] = Input::Key::F8;
    keyCodeMap[GLFW_KEY_F9] = Input::Key::F9;
    keyCodeMap[GLFW_KEY_F10] = Input::Key::F10;
    keyCodeMap[GLFW_KEY_F11] = Input::Key::F11;
    keyCodeMap[GLFW_KEY_F12] = Input::Key::F12;
    keyCodeMap[GLFW_KEY_F13] = Input::Key::F13;
    keyCodeMap[GLFW_KEY_F14] = Input::Key::F14;
    keyCodeMap[GLFW_KEY_F15] = Input::Key::F15;
    keyCodeMap[GLFW_KEY_F16] = Input::Key::F16;
    keyCodeMap[GLFW_KEY_F17] = Input::Key::F17;
    keyCodeMap[GLFW_KEY_F18] = Input::Key::F18;
    keyCodeMap[GLFW_KEY_F19] = Input::Key::F19;
    keyCodeMap[GLFW_KEY_F20] = Input::Key::F20;
    keyCodeMap[GLFW_KEY_F21] = Input::Key::F21;
    keyCodeMap[GLFW_KEY_F22] = Input::Key::F22;
    keyCodeMap[GLFW_KEY_F23] = Input::Key::F23;
    keyCodeMap[GLFW_KEY_F24] = Input::Key::F24;
    keyCodeMap[GLFW_KEY_NUM_LOCK] = Input::Key::Numlock;
    keyCodeMap[GLFW_KEY_SCROLL_LOCK] = Input::Key::ScrollLock;
    keyCodeMap[GLFW_KEY_LEFT_SHIFT] = Input::Key::LeftShift;
    keyCodeMap[GLFW_KEY_RIGHT_SHIFT] = Input::Key::RightShift;
    keyCodeMap[GLFW_KEY_LEFT_CONTROL] = Input::Key::LeftControl;
    keyCodeMap[GLFW_KEY_RIGHT_CONTROL] = Input::Key::RightContol;
    keyCodeMap[GLFW_KEY_MENU] = Input::Key::Menu;
}

InputManagerGLFW::InputManagerGLFW(std::shared_ptr<InputEventHandler> pInputHandler) :
    InputManager(pInputHandler)
{
    makeKeyCodeMap();
}


void InputManagerGLFW::setWindowCallbacks(GLFWwindow* pWindow)
{
    windowManagerMap[pWindow] = this;
    glfwSetKeyCallback(pWindow, key_callback);
    glfwSetMouseButtonCallback(pWindow, mouse_button_callback);
    glfwSetCursorPosCallback(pWindow, cursor_position_callback);
}


void InputManagerGLFW::pollEvents() const
{
    glfwPollEvents();
}

