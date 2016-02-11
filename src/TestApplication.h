#pragma once

#include "BaseApplication.h"

// only needed for the camera picking
#include "glm/vec3.hpp"

#include <memory>

class Camera;
class Grid;

class TestApplication : public BaseApplication {
public:

	TestApplication();
	virtual ~TestApplication();

	virtual bool startup();
	virtual void shutdown();

	virtual bool update(float deltaTime);
	virtual void draw();

private:

	Camera*		m_camera;

	// this is an example position for camera picking
	glm::vec3	m_pickPosition;

	std::shared_ptr<Grid> m_pGrid;

};