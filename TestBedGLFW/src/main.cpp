#include "config.h"

#if defined USE_VLD
#include <vld.h>
#endif


#include "ApplicationGLFW.h"

#include "test\Test.h"



int main(int argc, char** argv) 
{
    std::shared_ptr<ApplicationGLFW> pApplication = std::make_shared<ApplicationGLFW>("TestBed", 1024, 768);

    if (!setupTestScene(pApplication->getEngine(), pApplication->getInputManager(), pApplication->getWindow().get()))
        return 2;

    if (!pApplication->startup()) 
        return 1;
     
    pApplication->run();

    pApplication->shutdown();

    return 0;
}

