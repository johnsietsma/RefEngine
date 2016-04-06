#include "WindowQT.h"

#include "InputManagerQT.h"

#include <Engine.h>
#include <engine/GameObjectManager.h>
#include <gameobjects/CameraGameObject.h>
#include <test/Test.h>

#include <QApplication>
#include <QKeyEvent>
#include <QOpenGLContext>
#include <QMainWindow>
#include <QScreen>

WindowQT::WindowQT(QWidget *parent) :
    QOpenGLWidget(parent),
    m_pEngine(std::make_shared<Engine>())
{  
    m_pInputManager = std::make_shared<InputManagerQT>(m_pEngine->getGameObjectManager().lock());

}

WindowQT::~WindowQT()
{
    m_pEngine->shutdown();
}

glm::ivec2 WindowQT::getFramebufferSize() const
{
    qreal devicePixelRatio = QApplication::primaryScreen()->devicePixelRatio();
    glm::ivec2 size = getWindowSize();
    return glm::ivec2(size.x*devicePixelRatio, size.y*devicePixelRatio);
}

glm::ivec2 WindowQT::getWindowSize() const
{
    return glm::ivec2(width(), height());
}

float WindowQT::getAspectRatio() const
{
    return width()/static_cast<float>(height());
}

void WindowQT::initializeGL()
{
    QWidget* pWidget = QApplication::activeWindow();
    if (pWidget != nullptr) {
        QVariant testBoolVariant = pWidget->property("testBool");
        bool testBool = testBoolVariant.value<bool>();
    }

    foreach(QWidget *widget, QApplication::topLevelWidgets()) {
        if (widget->objectName() == "MainWindow")
        {
            QVariant testBoolVariant = widget->property("TestProp");
            bool testBool = testBoolVariant.value<bool>();
        }
    }

    if (ogl_LoadFunctions() == ogl_LOAD_FAILED) {
        return;
    }

    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &WindowQT::cleanup);

    //std::shared_ptr<Window> pWindow(this); // Not ref counted correctly
    if(!setupTestScene(m_pEngine, m_pInputManager, this)) return;

    if (!m_pEngine->startup()) return;
}

void WindowQT::paintGL()
{
    makeCurrent();

    unsigned int fboId = context()->defaultFramebufferObject();
    unsigned int fboIdWidget = defaultFramebufferObject();

//    GLint prev_fbo_id;
    //glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prev_fbo_id);

    m_pEngine->update(0.03f);
    m_pEngine->draw(fboIdWidget);

    //glBindFramebuffer(GL_FRAMEBUFFER, prev_fbo_id);

    update(); // Schedule another draw
};

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
