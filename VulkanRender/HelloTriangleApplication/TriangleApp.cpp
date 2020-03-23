#include "TriangleApp.h"
#include <stdexcept>
#include <vector>
#include <cstring>
#include <iostream>

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
	void* userData)
{
	std::cerr << "Validation layer: " << callbackData->pMessage << std::endl;
	return VK_FALSE;
}

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
		if (enableValidationLayers && !checkValidationLayerSupport())
			throw std::runtime_error("Validation layers enabled but not available");
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
		if (enableValidationLayers)
		{
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();
		} 
		else createInfo.enabledLayerCount = 0;
		return vkCreateInstance(&createInfo, nullptr, &instance);
	}

	bool checkValidationLayerSupport()
	{
		uint32_t layersCount = 0;
		vkEnumerateInstanceLayerProperties(&layersCount, nullptr);
		std::vector<VkLayerProperties> availableLayers(layersCount);
		vkEnumerateInstanceLayerProperties(&layersCount, availableLayers.data());
		for (const char* layerName : validationLayers)
		{
			bool layerFound = false;
			for (VkLayerProperties layer : availableLayers)
				if (strcmp(layerName, layer.layerName) == 0)
					layerFound = true;
			if (!layerFound)
				return false;
		}
		return true;
	}

	std::vector<const char*> getRequiredExtensions()
	{
		uint32_t glfwExtensionsCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionsCount);
		std::vector<const char*> retExtensions(glfwExtensions, glfwExtensions + glfwExtensionsCount);
		if (enableValidationLayers)
			retExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		return retExtensions;
	}

	GLFWwindow* window;
	VkInstance instance;
	int WindowWidth;
	int WindowHeight;

	// configuration variables
	const std::vector<const char*> validationLayers =
	{
		"VK_LAYER_KHRONOS_validation"
	};

#ifdef _DEBUG
	const bool enableValidationLayers = true;
#else
	const bool enableValidationLayers = false;
#endif // _DEBUG

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

	uint32_t extensionsCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, nullptr);
	std::vector<VkExtensionProperties> extensions(extensionsCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, extensions.data());

#ifdef _DEBUG
	std::cout << "Available extensions:" << std::endl << std::endl;
	for (VkExtensionProperties extension : extensions)
		printf("Name: %s\nVersion: %d\n\n", extension.extensionName, extension.specVersion);
#endif // _DEBUG

	
	// ...
}

void TriangleApp::mainLoop()
{
	while (!glfwWindowShouldClose(d->window))
		glfwPollEvents();
}

void TriangleApp::cleanup()
{
	vkDestroyInstance(d->instance, nullptr);
	glfwDestroyWindow(d->window);
	glfwTerminate();
}
