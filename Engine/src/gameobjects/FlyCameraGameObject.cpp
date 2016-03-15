#include "FlyCameraGameObject.h"

#include "components/FlyComponent.h"

bool FlyCameraGameObject::create()
{
    CameraGameObject::create();
    addComponent<FlyComponent>( m_pInputManager );
    return true;
}
