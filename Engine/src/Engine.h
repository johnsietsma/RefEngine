#pragma once

#include "graphics/RenderPass.h"

#include <assert.h>
#include <memory>
#include <vector>

class Camera;
class GameObject;
class InputManager;
class Light;
class Window;


template<typename T>
using sp_vector = std::vector< std::shared_ptr<T> >;

/*
*/
class Engine : public std::enable_shared_from_this<Engine>
{
public:
    Engine( const char* pWindowName );
    ~Engine();

    void run();

    bool startup();
    void shutdown();

    bool update(float deltaTime);
    void draw();

    std::weak_ptr<Camera> getMainCamera() const { return m_pMainCamera;  }
    std::weak_ptr<Window> getWindow() const { return m_pWindow; }
    sp_vector<GameObject>& getGameObjects() { return m_gameObjects; }
    const sp_vector<GameObject>& getGameObjects() const { return m_gameObjects; }

    void addCamera( std::shared_ptr<Camera> pCamera ) { m_cameras.push_back( pCamera); }
    void addRenderPass(const RenderPass& renderPass);
    void addGameObject( std::shared_ptr<GameObject> pGameObject ) { m_gameObjects.push_back(pGameObject); }


private:
    std::shared_ptr<Camera> m_pMainCamera;

    sp_vector<Camera> m_cameras;
    sp_vector<GameObject> m_gameObjects;
    std::vector<RenderPass> m_renderPasses;

    std::shared_ptr<Window> m_pWindow;
    std::shared_ptr<InputManager> m_pInputManager;

    std::shared_ptr<Light> m_pLight;

    bool m_shouldDrawGrid;
};
