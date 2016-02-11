#include "TestApplication.h"
#include <GLFW/glfw3.h>

int main() {
	
	BaseApplication* app = new TestApplication();
	if (app->startup())
		app->run();
	app->shutdown();

	return 0;
}
