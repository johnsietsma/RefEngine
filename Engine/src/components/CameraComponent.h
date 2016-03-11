#include "engine/Component.h"

#include "engine/Camera.h"
#include "engine/GameObject.h"

class CameraComponent : public Component
{
public:
    CameraComponent( std::shared_ptr<GameObject> pGameObject, float fovY, float aspectRatio, float nearClip, float farClip) :
        Component( pGameObject ),
        m_camera(fovY, aspectRatio, nearClip, farClip)
    {}

    const Camera&       getCamera() const { return m_camera; }
    const Transform&    getTransform() const { return getGameObject().lock()->getTransform(); }
    const glm::mat4&    getProjectionTransform() const { return m_camera.getProjectionTransform(); }
    glm::mat4           getViewTransform() const { return glm::inverse(getTransform().getMatrix()); }
    glm::mat4           getProjectionViewTransform() const { return getProjectionTransform() * getViewTransform(); }
    void                getFrustumPlanes(glm::vec4(&pPlanes)[6]) const { return m_camera.getFrustumPlanes( getViewTransform(), pPlanes ); }

    void setCamara( const Camera camera ) { m_camera = camera;  }
    
private:
    Camera m_camera;
};
