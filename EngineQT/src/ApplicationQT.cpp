#include "ApplicationQT.h"

#include "Engine.h"
#include "engine/Window.h"
#include "input/InputManager.h"

#include "WindowQT.h"
#include "InputManagerQT.h"





ApplicationQT::ApplicationQT(int& argc, char** argv) :
    m_pApplication(argc, argv)
{
    m_timer.start();
}

float ApplicationQT::getTime() const
{
    return m_timer.elapsed();
}


void ApplicationQT::run() {

    m_pApplication.exec();
}
