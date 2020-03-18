#include "TriangleApp.h"
#include <stdexcept>


class TriangleApp::Private
{
public:
	Private()
	{
		window = nullptr;
		instance = NULL;
		WindowWidth = 800;
		WindowHeight = 600;
	}
	~Private()
	{ }
	VkResult createVkInstance()
	{
		VkApplicationInfo appInfo = {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Hello triangle";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "No Example";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;
		VkInstanceCreateInfo createInfo = {};
		createInfo.pApplicationInfo = &appInfo;
		uint32_t glfwExtCount = 0;
		char** glfwExt;
		glfwExt = (char**)glfwGetRequiredInstanceExtensions(&glfwExtCount);
		createInfo.enabledExtensionCount = glfwExtCount;
		createInfo.ppEnabledExtensionNames = glfwExt;
		createInfo.enabledLayerCount = 0;
		return vkCreateInstance(&createInfo, nullptr, &instance);
	}
	GLFWwindow* window;
	VkInstance instance;
	int WindowWidth;
	int WindowHeight;

};

TriangleApp::TriangleApp()
	: d( new Private())
{ }

TriangleApp::~TriangleApp()
{
	if (d)
		delete d;
}

void TriangleApp::run()
{
	initWindow();
	initVulkan();
	mainLoop();
	cleanup();
}

void TriangleApp::initWindow()
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	d->window = glfwCreateWindow( d->WindowWidth, d->WindowHeight, "Vulkan", nullptr, nullptr);

}

void TriangleApp::initVulkan()
{
	if (d->createVkInstance() != VK_SUCCESS)
		throw std::runtime_error("Failed to create instance!");
}

void TriangleApp::mainLoop()
{
	while (!glfwWindowShouldClose(d->window))
		glfwPollEvents();
}

void TriangleApp::cleanup()
{
	glfwDestroyWindow(d->window);
	glfwTerminate();
}
