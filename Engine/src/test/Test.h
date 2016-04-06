#pragma once

#include <memory>

class Engine;
class InputManager;
class Window;

bool setupTestScene(std::shared_ptr<Engine> pEngine, std::shared_ptr<InputManager> pInputManager, Window* pWindow);
