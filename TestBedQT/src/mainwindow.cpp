#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "Engine.h"

Q_DECLARE_METATYPE(std::shared_ptr<Engine>)


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_pEngine(std::make_shared<Engine>())
{
    qRegisterMetaType<std::shared_ptr<Engine>>();
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

