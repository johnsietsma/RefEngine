#include "engine/Component.h"

#include "engine/Transform.h"

class TransformComponent : public Component
{
public:
    TransformComponent( std::shared_ptr<GameObject> pGameObject ) :
        Component( pGameObject )
    {}

    const Transform& getTransform() const { return m_transform;  }
    void setTransform( const Transform& transform ) { m_transform = transform;  }
    
private:
    Transform m_transform;
};
