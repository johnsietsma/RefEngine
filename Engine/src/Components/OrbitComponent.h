#pragma once

#include "Engine/Component.h"

class Transform;

class OrbitComponent : public Component
{
public:
    OrbitComponent( std::weak_ptr<GameObject> pGameObject,  std::weak_ptr<Transform> pwTransform ) :
        Component( pGameObject ),
        m_pwTransform(pwTransform)
    {}

    void update( float deltaTime ) override;

private:
    std::weak_ptr<Transform> m_pwTransform;
};
