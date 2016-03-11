#pragma once

#include "engine/GameObject.h"

#include <memory>

class Camera;
class InputManager;

/*
*/
class FlyCameraGameObject : public GameObject
{
public:
    FlyCameraGameObject(const Transform& trans, std::shared_ptr<InputManager> pInputManager) :
        GameObject(trans)
    {}

    bool create() override;

private:
    std::shared_ptr<Camera> m_camera;

};

