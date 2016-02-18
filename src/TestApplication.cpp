#include "TestApplication.h"
#include "gl_core_4_4.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "Engine/Camera.h"
#include "GameObjects/GameObjects.h"

#include "Gizmos.h"

using glm::vec3;
using glm::vec4;
using glm::mat4;

TestApplication::TestApplication()
{

}

TestApplication::~TestApplication() {

}

bool TestApplication::startup() {

	// create a basic window
	const glm::ivec2 windowSize(1024, 768);
	createWindow("AIE OpenGL Application", windowSize.x, windowSize.y);

	// start the gizmo system that can draw basic shapes
	Gizmos::create();

	// create a camera
	m_pCamera = std::make_shared<Camera>(glm::radians(45.f), windowSize.x/(float)windowSize.y, 0.1f, 1000.f);
	m_pCamera->setLookAtFrom(vec3(0, 10, 10), vec3(0));

    Transform fbxTransform = Transform(glm::vec3(0, 0, -2), glm::quat(), glm::vec3(0.01f));

    ParticleEmitterConfig config;
    config.emitRate = 500;
    config.startColor = glm::vec4(1, 0, 0, 1);
    config.endColor = glm::vec4(1, 1, 0, 1);
    config.lifespanMin = 0.1f;
    config.lifespanMax = 5;
    config.particleCount = 1000;
    config.startSize = 1.f;
    config.endSize = 0.1f;
    config.velocityMin = 0.1f;
    config.velocityMax = 1.0f;

    //m_gameObjects.emplace_back(std::make_shared<FBXMeshGameObject>(fbxTransform, "./data/models/Pyro/pyro.fbx"));
    //m_gameObjects.emplace_back(std::make_shared<ParticleEmitterGameObject>(config, m_pCamera.get()));
    //m_gameObjects.emplace_back(std::make_shared<SpriteSheetQuadGameObject>(glm::vec3(-3, 0.02f, -3), "./data/textures/spritesheet.png", 4, 4));
    //m_gameObjects.emplace_back(std::make_shared<TexturedQuadGameObject>(glm::vec3(3, 0.02f, -3), "./data/textures/crate.png"));
    //m_gameObjects.emplace_back(std::make_shared<VertexColoredGridGameObject>(glm::vec3(0, 0.01f, 2), glm::ivec2(5, 5)));
    m_gameObjects.emplace_back(std::make_shared<ProceduralGenerationGameObject>(glm::vec3(0)));


    for (auto& gameObject : m_gameObjects)
    {
        if (!gameObject->create()) return false;
    }

	return true;
}

void TestApplication::shutdown() 
{

    for (auto& gameObject : m_gameObjects)
    {
        gameObject->destroy();
    }
    m_gameObjects.clear();

	//m_pVertexColoredGrid->destroy();
	//m_pSpriteSheetQuad->destroy();
	//m_pParticleEmitter->destroy();

	// delete our camera and cleanup gizmos
	Gizmos::destroy();

	// destroy our window properly
	destroyWindow();
}

bool TestApplication::update(float deltaTime) {
	
	// close the application if the window closes
	if (glfwWindowShouldClose(m_window) ||
		glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		return false;

	// update the camera's movement
	m_pCamera->update(deltaTime);

    for (auto& gameObject : m_gameObjects)
    {
        gameObject->update(deltaTime);
    }

	//m_pSpriteSheetQuad->update(deltaTime);

	//m_pFBXMesh->update(deltaTime);

	//m_pParticleEmitter->update(deltaTime, m_camera->getTransform());

	// clear the gizmos out for this frame
	Gizmos::clear();

	Gizmos::addTransform(glm::mat4());

	// ...for now let's add a grid to the gizmos
	for (int i = 0; i < 21; ++i) {
		Gizmos::addLine(vec3(-10 + i, 0, 10), vec3(-10 + i, 0, -10),
			i == 10 ? vec4(1, 1, 1, 1) : vec4(0, 0, 0, 1));

		Gizmos::addLine(vec3(10, 0, -10 + i), vec3(-10, 0, -10 + i),
			i == 10 ? vec4(1, 1, 1, 1) : vec4(0, 0, 0, 1));
	}

	// return true, else the application closes
	return true;
}

void TestApplication::draw() {

	// clear the screen for this frame
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (auto& gameObject : m_gameObjects)
    {
        gameObject->draw(*m_pCamera);
    }
	//m_pVertexColoredGrid->draw(projView);
	//m_pSpriteSheetQuad->draw(projView);
	//m_pFBXMesh->draw(projView);
	//m_pParticleEmitter->draw(projView);

	// display the 3D gizmos
	Gizmos::draw(m_pCamera->getProjectionView());
}