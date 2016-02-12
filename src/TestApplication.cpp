#include "TestApplication.h"
#include "gl_core_4_4.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "Camera.h"
#include "TexturedQuad.h"
#include "VertexColoredGrid.h"
#include "Gizmos.h"

using glm::vec3;
using glm::vec4;
using glm::mat4;

TestApplication::TestApplication() : 
	m_camera(nullptr) ,
	m_pVertexColoredGrid( std::make_shared<VertexColoredGrid>() ),
	m_pTexturedQuad( std::make_shared<TexturedQuad>() )
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
	m_camera = new Camera(glm::radians(45.f), windowSize.x/(float)windowSize.y, 0.1f, 1000.f);
	m_camera->setLookAtFrom(vec3(0, 10, 10), vec3(0));
	
	m_pickPosition = glm::vec3(0);

	if (!m_pVertexColoredGrid->create( glm::vec3(2,0.001f,0), glm::ivec2(5,5)) ) return false;

	if (!m_pTexturedQuad->create(glm::vec3(-2, 0.0002f, 0))) return false;

	return true;
}

void TestApplication::shutdown() {

	m_pVertexColoredGrid->destroy();
	m_pTexturedQuad->destroy();

	// delete our camera and cleanup gizmos
	delete m_camera;
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
	m_camera->update(deltaTime);

	// clear the gizmos out for this frame
	Gizmos::clear();

	// an example of mouse picking
	if (glfwGetMouseButton(m_window, 0) == GLFW_PRESS) {
		double x = 0, y = 0;
		glfwGetCursorPos(m_window, &x, &y);

		// plane represents the ground, with a normal of (0,1,0) and a distance of 0 from (0,0,0)
		glm::vec4 plane(0, 1, 0, 0);
		m_pickPosition = m_camera->pickAgainstPlane((float)x, (float)y, plane);
	}

	Gizmos::addTransform(glm::translate(m_pickPosition));

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

	glm::mat4 projView = m_camera->getProjectionView();
	m_pVertexColoredGrid->draw(projView);
	m_pTexturedQuad->draw(projView);

	// display the 3D gizmos
	Gizmos::draw(m_camera->getProjectionView());

	// get a orthographic projection matrix and draw 2D gizmos
	int width = 0, height = 0;
	glfwGetWindowSize(m_window, &width, &height);
	mat4 guiMatrix = glm::ortho<float>(0, 0, (float)width, (float)height);

	Gizmos::draw2D(m_camera->getProjectionView());
}