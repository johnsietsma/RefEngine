#include <vld.h>

#include "TestApplication.h"
#include <GLFW/glfw3.h>

int main() {
	
	BaseApplication* app = new TestApplication();
	if (app->startup())
		app->run();
	app->shutdown();

	delete app;

	return 0;
}
