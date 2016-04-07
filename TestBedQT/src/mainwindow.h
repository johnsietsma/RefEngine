#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>

#include <QWidget>
#include <QMainWindow>

class Engine;
class InputManagerQT;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    Q_PROPERTY(std::shared_ptr<Engine> RefEngine READ getRefEngine)
    Q_PROPERTY(std::shared_ptr<InputManagerQT> InputManager READ getInputManager)

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    std::shared_ptr<Engine>             getRefEngine() const        { return m_pEngine; }
    std::shared_ptr<InputManagerQT>     getInputManager() const     { return m_pInputManager; }

private:
    Ui::MainWindow *ui;
    std::shared_ptr<Engine> m_pEngine;
    std::shared_ptr<InputManagerQT> m_pInputManager;
};

#endif // MAINWINDOW_H
