#include "BaseApplication.h"
#include "Engine/Helpers.h"

#include "gl_core_4_4.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <assert.h>
#include <iostream>

BaseApplication::~BaseApplication()
{
	assert(m_window == nullptr && "destroyWindow has not been called.");
}


bool BaseApplication::createWindow(const char* title, int width, int height) {
	assert(m_window == nullptr);


	if (glfwInit() == GL_FALSE)
		return false;

    // Required to get MacOS out of compatiblity mode
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	m_window = glfwCreateWindow(width, height, title, nullptr, nullptr);
	if (m_window == nullptr) {
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(m_window);

	if (ogl_LoadFunctions() == ogl_LOAD_FAILED) {
		glfwDestroyWindow(m_window);
		glfwTerminate();
		return false;
	}

	glfwSetWindowSizeCallback(m_window, [](GLFWwindow*, int w, int h){ glViewport(0, 0, w, h); });

	auto major = ogl_GetMajorVersion();
	auto minor = ogl_GetMinorVersion();
	std::cout << "GL: " << major << "." << minor << std::endl;

	glClearColor(0.25f, 0.25f, 0.25f, 1);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

#ifdef _DEBUG
	TurnOnOpenGLDebugLogging();
#endif

	return true;
}

void BaseApplication::destroyWindow() {

	glfwDestroyWindow(m_window);
	glfwTerminate();
	m_window = nullptr;
}

void BaseApplication::run() {

	double prevTime = glfwGetTime();
	double currTime = 0;

	while (currTime = glfwGetTime(),
		update((float)(currTime - prevTime))) {

		glfwPollEvents();
		draw();
		glfwSwapBuffers(m_window);

		prevTime = currTime;
	}
}
