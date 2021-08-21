#include "TriangleApp.h"
#include <stdexcept>
#include <cstring>
#include <iostream>
#include <map>

class TriangleApp::Private
{
public:
	Private()
	{
		Window = nullptr;
		WindowWidth = 800;
		WindowHeight = 600;
	}

	~Private()
	{ }

	GLFWwindow* Window;
	int WindowWidth;
	int WindowHeight;
};

TriangleApp::TriangleApp()
	: d( new Private()), VulkanApp()
{ }

TriangleApp::~TriangleApp()
{
	if (d)
		delete d;
}

void TriangleApp::run()
{
	initWindow();
	InitVulkan();
	mainLoop();
	cleanup();
}

std::vector<const char*> TriangleApp::GetRequiredExtensions()
{
	uint32_t glfwExtensionsCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionsCount);
	std::vector<const char*> retExtensions(glfwExtensions, glfwExtensions + glfwExtensionsCount);
	if (enableValidationLayers)
		retExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	return retExtensions;
}

void TriangleApp::initWindow()
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	d->Window = glfwCreateWindow( d->WindowWidth, d->WindowHeight, "Vulkan", nullptr, nullptr);
}

void TriangleApp::mainLoop()
{
	while (!glfwWindowShouldClose(d->Window))
		glfwPollEvents();
}

void TriangleApp::cleanup()
{
	CleanupVulkan();
	glfwDestroyWindow(d->Window);
	glfwTerminate();
}
