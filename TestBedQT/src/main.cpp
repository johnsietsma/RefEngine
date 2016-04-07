#include "config.h"

#if defined USE_VLD
#include <vld.h>
#endif

#include "mainwindow.h"

#include <QApplication>
#include <QSurfaceFormat>

int main(int argc, char** argv) 
{
    QApplication a(argc, argv);

    // Specify the format and create platform-specific surface
    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setMajorVersion(3);
    format.setMinorVersion(3);
//    format.setSamples(4);
    format.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(format);


    MainWindow mainWindow;
    mainWindow.resize(mainWindow.sizeHint());
    mainWindow.show();

    return a.exec();
}

