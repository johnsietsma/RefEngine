#include "WindowGLFW.h"

#include "gl_core_4_4.h"

#include <GLFW/glfw3.h>


WindowGLFW::WindowGLFW(const char* title, int width, int height) :
    m_pWindow(nullptr)
{
    if (glfwInit() == GL_FALSE)
        return;

    // Required to get MacOS out of compatiblity mode
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    m_pWindow = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (m_pWindow == nullptr) {
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(m_pWindow);

    if (ogl_LoadFunctions() == ogl_LOAD_FAILED) {
        glfwDestroyWindow( m_pWindow );
        glfwTerminate();
        m_pWindow = nullptr;
        return;
    }

    glfwSetWindowSizeCallback(m_pWindow, [](GLFWwindow*, int w, int h)
        {
            glViewport(0, 0, w, h);
        });
}

WindowGLFW::~WindowGLFW()
{
    if( m_pWindow!=nullptr ) glfwDestroyWindow(m_pWindow);
    glfwTerminate();
    m_pWindow = nullptr;
}

bool WindowGLFW::shouldClose() const 
{ 
    return glfwWindowShouldClose(m_pWindow); 
}


float WindowGLFW::getTime() const
{
    return (float)glfwGetTime();
}

void WindowGLFW::swapBuffers() const
{
    glfwSwapBuffers( m_pWindow );
}

glm::ivec2 WindowGLFW::getFramebufferSize() const
{
    int width, height;
    glfwGetFramebufferSize(m_pWindow, &width, &height);
    return glm::ivec2( width, height );
}

glm::ivec2 WindowGLFW::getWindowSize() const
{
    int width, height;
    glfwGetWindowSize(m_pWindow, &width, &height);
    return glm::ivec2( width, height );
}

float WindowGLFW::getAspectRatio() const
{
    int width, height;
    glfwGetWindowSize(m_pWindow, &width, &height);
    return width/(float)height;
}
