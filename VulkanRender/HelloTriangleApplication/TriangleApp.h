#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class TriangleApp
{
public:

	TriangleApp();
	~TriangleApp();

	void run();

private:
	class Private;
	Private* d;
	
	void initWindow();
	void initVulkan();
	void mainLoop();
	void cleanup();
};