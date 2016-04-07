#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "Engine.h"
#include "InputManagerQT.h"
#include "engine/GameObjectManager.h"

Q_DECLARE_METATYPE(std::shared_ptr<Engine>)
Q_DECLARE_METATYPE(std::shared_ptr<InputManagerQT>)


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_pEngine(std::make_shared<Engine>())
{
    qRegisterMetaType<std::shared_ptr<Engine>>();
    qRegisterMetaType<std::shared_ptr<InputManagerQT>>();

    m_pInputManager = std::make_shared<InputManagerQT>(m_pEngine->getGameObjectManager().lock());
    installEventFilter(m_pInputManager.get());

    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

