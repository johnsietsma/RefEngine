#include "engine/Component.h"

#include <vector>

class GameObject;
class Program;

class RenderModeComponent : public Component
{
public:
    RenderModeComponent( std::shared_ptr<GameObject> pGameObject, std::vector<Program>& programs ) :
        Component( pGameObject ),
        m_programs(programs)
    {}

    virtual void onKeyEvent(Input::Key key, Input::Action action) override;

private:
    std::vector<Program> m_programs;
};
