#pragma once

#include "BaseApplication.h"

#include "Engine/RenderPass.h"

#include <memory>
#include <vector>

class Camera;
class GameObject;

class TestApplication : public BaseApplication {
public:

    TestApplication();
    virtual ~TestApplication();

    virtual bool startup();
    virtual void shutdown();

    virtual bool update(float deltaTime) override;
    virtual void draw();

private:
    std::shared_ptr<Camera> m_pCamera;
    std::shared_ptr<Camera> m_pFBOCamera;
    std::vector< std::shared_ptr<GameObject> > m_gameObjects;
    std::vector<RenderPass> m_renderPasses;
};