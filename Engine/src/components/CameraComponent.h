#include "engine/Component.h"

#include "engine/Camera.h"
#include "engine/GameObject.h"

class CameraComponent : public Component
{
public:
    CameraComponent( std::shared_ptr<GameObject> pGameObject, CameraConfig config) :
        Component( pGameObject ),
        m_camera( config )
    {}

    const Camera& getCamera() const { return m_camera; }
    void setCamera( const Camera camera ) { m_camera = camera;  }
    
private:
    Camera m_camera;
};
