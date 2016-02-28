#include "Engine/Component.h"

#include "gl_core_4_4.h"

class GameObject;

class RenderModeComponent : public Component
{
public:
    RenderModeComponent( std::shared_ptr<GameObject> pGameObject ) :
        Component( pGameObject )
    {}

    virtual void OnKeyEvent( Input::Key key ) override
    {
        auto pGameObject = getGameObject().lock();
        if( pGameObject!=nullptr ) {
            pGameObject->getRenderablesVisitor().visit( [](Renderable& renderable) { renderable.renderMode = GL_LINE; });
        }
    }
};
