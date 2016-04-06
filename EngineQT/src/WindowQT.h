#pragma once

#include <graphics/gl_core_3_3.h>
#include <engine/Window.h>

#include <glm/vec2.hpp>

#include <memory>

#undef QT_OPENGL_ES_3
#include <QOpenGLWidget>

class Engine;
class InputManager;
class QWidget;

class WindowQT : public QOpenGLWidget, public Window
{
    Q_OBJECT

public:
    WindowQT(QWidget* pQMainWindow);
    ~WindowQT();

    // ---- Window overrides ----
    bool isValid() const override { return true; }

    // The frame buffer size in pixels.
    // Can be bigger then the screen on Retina displays.
    glm::ivec2 getFramebufferSize() const override;
    
    // The window size in pixels
    glm::ivec2 getWindowSize() const override;
    float getAspectRatio() const override;
    
    // QT interface
    //QSize minimumSizeHint() const Q_DECL_OVERRIDE { return QSize(512, 512); };
    //QSize sizeHint() const Q_DECL_OVERRIDE { return QSize(512, 512); };
    //void rotateBy(int xAngle, int yAngle, int zAngle);
    //void setClearColor(const QColor &color);

    void cleanup() {};


signals:
    void clicked();

protected:
    void initializeGL() Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;

    void keyPressEvent(QKeyEvent *) Q_DECL_OVERRIDE;
    void keyReleaseEvent(QKeyEvent *) Q_DECL_OVERRIDE;

    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

private:
    //QMainWindow* pMainWindow;
    std::shared_ptr<Engine> m_pEngine;
    std::shared_ptr<InputManager> m_pInputManager;
};
