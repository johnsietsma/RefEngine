#pragma once

#include "graphics/RenderPass.h"

#include <assert.h>
#include <memory>
#include <vector>

class CameraGameObject;
class GameObjectManager;
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
    Engine( std::shared_ptr<Window> pWindow, std::shared_ptr<InputManager> pInputManager );
    ~Engine();

    void run();

    bool startup();
    void shutdown();

    bool update(float deltaTime);
    void draw();

    const std::weak_ptr<CameraGameObject> getMainCamera() const { return m_pMainCamera;  }
    const std::weak_ptr<Window> getWindow() const { return m_pWindow; }

    std::weak_ptr<GameObjectManager> getGameObjectManager() { return m_pGameObjectManager; }

    void addCamera( std::shared_ptr<CameraGameObject> pCamera ) { m_cameras.push_back( pCamera ); }
    void addRenderPass(const RenderPass& renderPass);


private:
    std::shared_ptr<CameraGameObject> m_pMainCamera;

    sp_vector<CameraGameObject> m_cameras; // Cache for camera GOs
    std::vector<RenderPass> m_renderPasses;

    std::shared_ptr<Window> m_pWindow;
    std::shared_ptr<InputManager> m_pInputManager;
    std::shared_ptr<GameObjectManager> m_pGameObjectManager;

    std::shared_ptr<Light> m_pLight;

    bool m_shouldDrawGrid;
};
