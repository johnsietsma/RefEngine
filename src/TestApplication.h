#pragma once

#include "BaseApplication.h"
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
    std::vector< std::shared_ptr<GameObject> > m_gameObjects;

    /*
	std::shared_ptr<VertexColoredGrid> m_pVertexColoredGrid;
	std::shared_ptr<SpriteSheetQuad> m_pSpriteSheetQuad;
	std::shared_ptr<FBXMesh> m_pFBXMesh;
	std::shared_ptr<ParticleEmitter> m_pParticleEmitter;
    */
};