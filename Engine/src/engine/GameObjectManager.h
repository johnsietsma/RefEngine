#pragma once

#include "input/InputEventHandler.h"

#include <bitset>
#include <memory>
#include <vector>

class CameraGameObject;
class GameObject;
class Light;
class Program;

class GameObjectManager : public InputEventHandler
{
public:
    bool create();
    void update(float deltaTime);
    void draw(const CameraGameObject& camera, const Light* pLight, const Program& overrideProgram, const std::bitset<8>& layers);
    void destroy();

    void addGameObject(std::shared_ptr<GameObject> pGameObject);

    void onKeyEvent(Input::Key key, Input::Action action) override;
    void onMouseButton(Input::MouseButton button, Input::Action action) override;

private:
    std::vector< std::shared_ptr<GameObject> > m_gameObjects;

};