#include "config.h"

#if defined USE_VLD
#include <vld.h>
#endif

#include "WindowQT.h"
#include "mainwindow.h"

#include <QApplication>

int main(int argc, char** argv) 
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}

