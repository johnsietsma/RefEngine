#include "WindowQT.h"

#include <QKeyEvent>

WindowQT::WindowQT(QWidget *parent) :
    QOpenGLWidget(parent)
{
    m_timer.start();

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
    return m_timer.elapsed();
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

void WindowQT::keyPressEvent(QKeyEvent * pEvent)
{
    Qt::Key key = (Qt::Key)pEvent->key();
}

void WindowQT::keyReleaseEvent(QKeyEvent *pEvent)
{
}

void WindowQT::mousePressEvent(QMouseEvent *event){}
void WindowQT::mouseMoveEvent(QMouseEvent *event){}
void WindowQT::mouseReleaseEvent(QMouseEvent *event){}
