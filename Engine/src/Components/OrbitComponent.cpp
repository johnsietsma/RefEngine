#include "OrbitComponent.h"

#include "engine/Transform.h"


void OrbitComponent::update( float deltaTime )
{
    auto pTransform = m_pwTransform.lock();
    if( pTransform==nullptr ) return;

    pTransform->rotate( 10.f * deltaTime );
}

