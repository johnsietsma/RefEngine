#include "OrbitComponent.h"

#include "Engine/Transform.h"


const glm::vec3 Transform::WORLD_UP = glm::vec3(0,1,0);

void OrbitComponent::update( float deltaTime )
{
    auto pTransform = m_pwTransform.lock();
    if( pTransform==nullptr ) return;

    pTransform->rotate( 10.f * deltaTime );
}

