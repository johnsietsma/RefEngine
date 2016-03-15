#pragma once

#include "components/CameraComponent.h"
#include "engine/GameObject.h"
#include <memory>
class Camera;

/*
*/
class CameraGameObject : public GameObject
{
public:
    CameraGameObject(const Transform& trans, float fovY, float aspectRatio, float near, float far) :
        GameObject(trans),
        m_config( fovY, aspectRatio, near, far ),
        m_cameraComponent(nullptr)
    {
    }
    
    const glm::mat4&    getProjectionTransform() const { return m_cameraComponent->getCamera().getProjectionTransform(); }
    glm::mat4           getViewTransform() const { return glm::inverse(getTransform().getMatrix()); }
    glm::mat4           getProjectionViewTransform() const { return getProjectionTransform() * getViewTransform(); }
    void                getFrustumPlanes(glm::vec4(&pPlanes)[6]) const {
        auto& camera = m_cameraComponent->getCamera();
        return camera.getFrustumPlanes( getViewTransform(), pPlanes ); }
    
    bool create() override
    {
        m_cameraComponent = addComponent<CameraComponent>( m_config );
        return true;
    }

private:
    CameraConfig m_config;
    std::shared_ptr<CameraComponent> m_cameraComponent;

};

