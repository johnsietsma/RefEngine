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
class Engine
{
public:
    Engine();
    ~Engine();

    bool startup();
    void shutdown();

    void update(float deltaTime);
    void draw(GLuint backbufferId=0);

    const std::shared_ptr<CameraGameObject> getMainCamera() const { assert(m_cameras.size()>0); return m_cameras[0]; }

    std::weak_ptr<GameObjectManager> getGameObjectManager() { return m_pGameObjectManager; }

    void addCamera( std::shared_ptr<CameraGameObject> pCamera ) { m_cameras.push_back( pCamera ); }
    void addRenderPass(const RenderPass& renderPass);
    void setLight(const std::shared_ptr<Light>& light) { m_pLight = light; }


private:
    sp_vector<CameraGameObject> m_cameras; // Cache for camera GOs
    std::vector<RenderPass> m_renderPasses;

    std::shared_ptr<GameObjectManager> m_pGameObjectManager;

    std::shared_ptr<Light> m_pLight;

    bool m_shouldDrawGrid;
};
