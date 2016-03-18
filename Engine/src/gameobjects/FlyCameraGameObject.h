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
            float fovY, float aspectRatio, float nearClip, float farClip) :
        CameraGameObject(trans, fovY, aspectRatio, nearClip, farClip)
    {
    }
    
    bool create() override;

private:
    std::shared_ptr<InputManager> m_pInputManager;
};

