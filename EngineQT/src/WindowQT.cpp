#include "WindowQT.h"

#include "gl_core_4_4.h"

WindowQT::WindowQT(QWidget *parent) :
    QOpenGLWidget(parent),
    m_pWindow(nullptr)
{
    // Specify the format and create platform-specific surface
    QSurfaceFormat format;
    format.setDepthBufferSize( 24 );
    format.setMajorVersion( 4 );
    format.setMinorVersion( 1 );
    format.setSamples( 4 );
    format.setProfile( QSurfaceFormat::CoreProfile );
    setFormat( format );
    QSurfaceFormat::setDefaultFormat(format);
}

WindowQT::~WindowQT()
{
}

float WindowQT::getTime() const
{
    return (float)glfwGetTime();
}

void WindowQT::swapBuffers() const
{
}

glm::ivec2 WindowQT::getFramebufferSize() const
{
    return glm::ivec2(0);
}

glm::ivec2 WindowQT::getWindowSize() const
{
    return glm::ivec2(0);
}

float WindowQT::getAspectRatio() const
{
    return 0;
}
