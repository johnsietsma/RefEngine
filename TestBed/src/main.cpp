#include "config.h"

#if defined USE_VLD
#include <vld.h>
#endif

#include "TestApplication.h"
#include <GLFW/glfw3.h>


int main() {
    BaseApplication* app = new TestApplication();
    if (!app->startup()) return 1;
    
    app->run();
    app->shutdown();

    delete app;

    return 0;
}
