#include "engine/Component.h"

#include "gl_core_4_4.h"

#include <algorithm>
#include <vector>

class GameObject;

class RenderModeComponent : public Component
{
public:
    RenderModeComponent( std::shared_ptr<GameObject> pGameObject, std::vector<Program>& programs ) :
        Component( pGameObject ),
        m_programs(programs)
    {}

    virtual void onKeyEvent( Input::Key key, Input::Action action) override
    {
        if (action != Input::Action::Press) return;
        if( (int)key < (int)Input::Key::Zero && (int)key > (int)Input::Key::Nine ) return;

        GLenum mode=  GL_FILL;
        Program program;

        int numPressed = -1;
        switch(key)
        {
            case Input::Key::One:
                mode = GL_FILL; break;
            case Input::Key::Two:
                mode = GL_LINE; break;
            case Input::Key::Three:
                mode = GL_POINT; break;
            case Input::Key::Four:
                numPressed = 4; break;
            case Input::Key::Five:
                numPressed = 5; break;
            case Input::Key::Six:
                numPressed = 6; break;
            case Input::Key::Seven:
                numPressed = 7; break;
            case Input::Key::Eight:
                numPressed = 8; break;
            case Input::Key::Nine:
                numPressed = 9; break;
            default:
                numPressed = -1; break;
        }

        if( numPressed!=-1 )
        {
            int programIndex = numPressed - 4;
            if( programIndex < m_programs.size())
            {
                program = m_programs[programIndex];
            }
        }

        auto pGameObject = getGameObject().lock();
        if( pGameObject!=nullptr ) {
            auto& renderables = pGameObject->getRenderables();
            std::for_each(  renderables.begin(), renderables.end(), [&mode, &program](Renderable& renderable) {
                renderable.renderMode = mode;
                if( program.isValid() ) renderable.program = program;
            });
        }
    }

private:
    std::vector<Program> m_programs;
};
