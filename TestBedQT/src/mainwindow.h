#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    Q_PROPERTY(bool TestProp READ getTestBool)

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    bool getTestBool() { return true; }

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
