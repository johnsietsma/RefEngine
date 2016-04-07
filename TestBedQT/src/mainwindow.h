#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>

#include <QWidget>
#include <QMainWindow>

class Engine;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    Q_PROPERTY(std::shared_ptr<Engine> RefEngine READ getRefEngine)

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    std::shared_ptr<Engine> getRefEngine() {
        return m_pEngine;
    }

private:
    Ui::MainWindow *ui;
    std::shared_ptr<Engine> m_pEngine;
};

#endif // MAINWINDOW_H
