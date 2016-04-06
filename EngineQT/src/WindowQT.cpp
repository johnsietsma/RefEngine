#include "WindowQT.h"

#include <Engine.h>
#include <engine/GameObjectManager.h>
#include <gameobjects/CameraGameObject.h>

#include <QApplication>
#include <QKeyEvent>
#include <QMainWindow>

WindowQT::WindowQT(QWidget *parent) :
    QOpenGLWidget(parent),
    m_pEngine(std::make_shared<Engine>())
{  
    // Specify the format and create platform-specific surface
    QSurfaceFormat format;
    format.setDepthBufferSize( 24 );
    format.setMajorVersion( 3 );
    format.setMinorVersion( 3 );
    format.setSamples( 4 );
    format.setProfile( QSurfaceFormat::CoreProfile );
    setFormat( format );
    QSurfaceFormat::setDefaultFormat(format);
}

WindowQT::~WindowQT()
{
    m_pEngine->shutdown();
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

    auto pGameObjectManager = m_pEngine->getGameObjectManager().lock();

    glm::ivec2 size(500, 500);
    Transform camTransform(glm::vec3(2, 6, 13), glm::vec3(0));

    auto mainCamera = std::make_shared<CameraGameObject>(camTransform, glm::radians(45.f), size.x / (float)size.y, 0.1f, 1000.f);
    pGameObjectManager->addGameObject(mainCamera);
    m_pEngine->addCamera(mainCamera);

    RenderPass renderPass(mainCamera, glm::vec3(0.25f, 0.25f, 0.25f), size);
    m_pEngine->addRenderPass(renderPass);


    m_pEngine->startup();
}

void WindowQT::paintGL()
{
    m_pEngine->update(0.03f);
    m_pEngine->draw();
};

void WindowQT::resizeGL(int width, int height)
{
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
