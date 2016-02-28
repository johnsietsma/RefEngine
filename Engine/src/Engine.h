#pragma once

#include <assert.h>
#include <memory>
#include <vector>

class Camera;
class GameObject;
class Light;
class RenderPass;
class Window;


/*
*/
class Engine {
public:

    Engine( const char* pWindowName );
    ~Engine();

    void run();

    bool startup();
    void shutdown();

    bool update(float deltaTime);
    void draw();

    std::weak_ptr<Window> getWindow() const { return m_pWindow; }

    void addCamera( std::shared_ptr<Camera> pCamera ) { m_cameras.push_back( pCamera); }
    void addRenderPass( const RenderPass& renderPass )
    {
        assert( m_renderPasses.size() > 0 );
        // Leave the default RenderPass at the end, insert at 1 before the end
        m_renderPasses.insert(m_renderPasses.begin() + m_renderPasses.size() - 1, renderPass);
    }
    void addGameObject( std::shared_ptr<GameObject> pGameObject ) { m_gameObjects.push_back(pGameObject); }

private:
    std::shared_ptr<Camera> m_pMainCamera;

    std::vector< std::shared_ptr<Camera>> m_cameras;
    std::vector< std::shared_ptr<GameObject> > m_gameObjects;
    std::vector<RenderPass> m_renderPasses;

    std::shared_ptr<Window> m_pWindow;

    std::shared_ptr<Light> m_pLight;

    bool m_shouldDrawGrid;
};
