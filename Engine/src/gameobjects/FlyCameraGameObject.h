#pragma once

#include "gameObjects/CameraGameObject.h"
#include <memory>

class Camera;
class InputManager;

/*
*/
class FlyCameraGameObject : public CameraGameObject
{
public:
    FlyCameraGameObject(const Transform& trans, std::shared_ptr<InputManager> pInputManager,
            float fovY, float aspectRatio, float near, float far) :
        CameraGameObject(trans, fovY, aspectRatio, near, far)
    {
    }
    
    bool create() override;

private:
    std::shared_ptr<InputManager> m_pInputManager;
};

