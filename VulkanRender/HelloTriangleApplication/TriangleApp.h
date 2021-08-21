#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "VulkanApp.h"
#include <vector>

class TriangleApp : public VulkanApp
{
public:

	TriangleApp();
	~TriangleApp();

	void run();

// VulkanApp
	virtual std::vector<const char*> GetRequiredExtensions();
	virtual const char* GetApplicationName() { return "Hello triangle"; }

private:
	class Private;
	Private* d;

	void initWindow();
	void mainLoop();
	void cleanup();
};