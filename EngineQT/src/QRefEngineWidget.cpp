#include "QRefEngineWidget.h"

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

Q_DECLARE_METATYPE(std::shared_ptr<Engine>)


QRefEngineWidget::QRefEngineWidget(QWidget *parent) :
    QOpenGLWidget(parent),
    m_pEngine(nullptr)
{  
    setFocus();
}

QRefEngineWidget::~QRefEngineWidget()
{
    m_pEngine->shutdown();
}

glm::ivec2 QRefEngineWidget::getFramebufferSize() const
{
    qreal devicePixelRatio = QApplication::primaryScreen()->devicePixelRatio();
    glm::ivec2 size = getWindowSize();
    return glm::ivec2(size.x*devicePixelRatio, size.y*devicePixelRatio);
}

glm::ivec2 QRefEngineWidget::getWindowSize() const
{
    return glm::ivec2(width(), height());
}

float QRefEngineWidget::getAspectRatio() const
{
    return width()/static_cast<float>(height());
}

void QRefEngineWidget::initializeGL()
{
    // Try to find the shared engine from the main window
    foreach(QWidget *widget, QApplication::topLevelWidgets()) {
        if (widget->objectName() == "MainWindow")
        {
            QVariant refEngineVariant = widget->property("RefEngine");
            if (refEngineVariant.isValid()) {
                m_pEngine = refEngineVariant.value<std::shared_ptr<Engine>>();
            }
        }
    }

    // Not found, make our own engine.
    if (m_pEngine == nullptr) {
        m_pEngine = std::make_shared<Engine>();
    }

    m_pInputManager = std::make_shared<InputManagerQT>(m_pEngine->getGameObjectManager().lock());

    if (ogl_LoadFunctions() == ogl_LOAD_FAILED) {
        return;
    }

    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &QRefEngineWidget::cleanup);

    //std::shared_ptr<Window> pWindow(this); // Not ref counted correctly
    if(!setupTestScene(m_pEngine, m_pInputManager, this)) return;

    if (!m_pEngine->startup()) return;
}

void QRefEngineWidget::paintGL()
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

void QRefEngineWidget::keyPressEvent(QKeyEvent * pEvent)
{
    Qt::Key key = (Qt::Key)pEvent->key();

    QOpenGLWidget::keyPressEvent(pEvent);
}

void QRefEngineWidget::keyReleaseEvent(QKeyEvent *pEvent)
{
    QOpenGLWidget::keyReleaseEvent(pEvent);

}

void QRefEngineWidget::mousePressEvent(QMouseEvent *pEvent)
{
    QOpenGLWidget::mousePressEvent(pEvent);
}

void QRefEngineWidget::mouseMoveEvent(QMouseEvent *pEvent)
{
    QOpenGLWidget::mouseMoveEvent(pEvent);

}

void QRefEngineWidget::mouseReleaseEvent(QMouseEvent *pEvent)
{
    QOpenGLWidget::mouseReleaseEvent(pEvent);
}
