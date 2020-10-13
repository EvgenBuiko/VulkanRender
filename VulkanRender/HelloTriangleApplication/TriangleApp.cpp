#include "TriangleApp.h"
#include <stdexcept>
#include <cstring>
#include <iostream>
#include <map>

/*static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
	void* userData)
{
	if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
		std::cerr << "Validation layer: " << callbackData->pMessage << std::endl;
	return VK_FALSE;
}

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance,
	const VkDebugUtilsMessengerCreateInfoEXT* createInfo,
	const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pCallback )
{
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (!func)
		return VK_ERROR_EXTENSION_NOT_PRESENT;

	return func(instance, createInfo, pAllocator, pCallback);
}

void DestroyDebugUtilsMessenger(VkInstance instance, VkDebugUtilsMessengerEXT callback,
	const VkAllocationCallbacks* pAllocator)
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func)
		func(instance, callback, pAllocator);
}*/

class TriangleApp::Private
{
public:
	Private()
	{
		Window = nullptr;
// 		Instance = NULL;
// 		DebugMessenger = NULL;
// 		PhysicalDevice = VK_NULL_HANDLE;
		WindowWidth = 800;
		WindowHeight = 600;
	}

	~Private()
	{ }

	/*VkResult createVkInstance();
	bool checkValidationLayerSupport();
	std::vector<const char*> getRequiredExtensions();
	void setupDebugMessenger();
	void pickPhysicalDevice();
	int rateDeviceSuitability(VkPhysicalDevice device);
	QueueFamilyIndices findQueueFamillies(VkPhysicalDevice device);
	bool isDeviceSuitable(VkPhysicalDevice device);
	void createLogicalDevice();

	VkInstance Instance;
	VkDevice Device;
	VkQueue GraphicsQueue;
	VkDebugUtilsMessengerEXT DebugMessenger;
	VkPhysicalDevice PhysicalDevice;*/

	GLFWwindow* Window;
	int WindowWidth;
	int WindowHeight;
	/*
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
	*/
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
	InitVulkan();
	//initVulkan();
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

/*void TriangleApp::initVulkan()
{
	if (d->createVkInstance() != VK_SUCCESS)
		throw std::runtime_error("Failed to create instance!");

#ifdef _DEBUG
	uint32_t extensionsCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, nullptr);
	std::vector<VkExtensionProperties> extensions(extensionsCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, extensions.data());

	std::cout << "Available extensions:" << std::endl << std::endl;
	for (VkExtensionProperties extension : extensions)
		printf("Name: %s\nVersion: %d\n\n", extension.extensionName, extension.specVersion);
#endif // _DEBUG

	d->setupDebugMessenger();
	d->pickPhysicalDevice();
	d->createLogicalDevice();

	// ...
}

void TriangleApp::Private::setupDebugMessenger()
{
	if (!enableValidationLayers) 
		return;

	VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity =
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
		| VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debugCallback;
	createInfo.pUserData = nullptr;

	if (CreateDebugUtilsMessengerEXT(Instance, &createInfo, nullptr, &DebugMessenger) != VK_SUCCESS)
		throw std::runtime_error("Failed to set up debug callback");
}

std::vector<const char*> TriangleApp::Private::getRequiredExtensions()
{
	uint32_t glfwExtensionsCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionsCount);
	std::vector<const char*> retExtensions(glfwExtensions, glfwExtensions + glfwExtensionsCount);
	if (enableValidationLayers)
		retExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	return retExtensions;
}

VkResult TriangleApp::Private::createVkInstance()
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
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	auto glfwExt = getRequiredExtensions();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(glfwExt.size());
	createInfo.ppEnabledExtensionNames = glfwExt.data();
	
	if (enableValidationLayers)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else createInfo.enabledLayerCount = 0;
	
	return vkCreateInstance(&createInfo, nullptr, &Instance);
}

bool TriangleApp::Private::checkValidationLayerSupport()
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

void TriangleApp::Private::pickPhysicalDevice()
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(Instance, &deviceCount, nullptr);

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(Instance, &deviceCount, devices.data());
	
	std::multimap<int, VkPhysicalDevice> candidates;
	for (const auto& device : devices)
	{
		int score = rateDeviceSuitability(device);
		candidates.insert(std::make_pair(score, device));
	}
	
	for (const auto& candidate : candidates)
	{
		if (candidate.first > 0 && isDeviceSuitable(candidate.second))
		{
			PhysicalDevice = candidate.second;
			break;
		}
	}

	if( PhysicalDevice == VK_NULL_HANDLE )
		throw std::runtime_error("Failed to find suitable GPU");
}

int TriangleApp::Private::rateDeviceSuitability(VkPhysicalDevice device)
{
	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
	
	int score = 0;
	if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		score += 1000;
	score += deviceProperties.limits.maxImageDimension2D;
	if (!deviceFeatures.geometryShader)
		return 0;

	return score;
}

QueueFamilyIndices TriangleApp::Private::findQueueFamillies(VkPhysicalDevice device)
{
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	for (int i = 0; i < queueFamilies.size(); i++)
	{
		if (queueFamilies[i].queueCount > 0 && queueFamilies[i].queueFlags == VK_QUEUE_GRAPHICS_BIT)
			indices.graphicsFamily = { i };
		if (indices.isComplete())
			break;
	}

	return indices;
}

bool TriangleApp::Private::isDeviceSuitable(VkPhysicalDevice device)
{
	QueueFamilyIndices indices = findQueueFamillies(device);
	return indices.isComplete();
}

void TriangleApp::Private::createLogicalDevice()
{
	QueueFamilyIndices indices = findQueueFamillies(PhysicalDevice);

	VkDeviceQueueCreateInfo queueCreateInfo = {};
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
	queueCreateInfo.queueCount = 1;
	float queuePriority = 1.0f;
	queueCreateInfo.pQueuePriorities = &queuePriority;

	VkPhysicalDeviceFeatures deviceFeatures = {};
	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = &queueCreateInfo;
	createInfo.queueCreateInfoCount = 1;
	createInfo.pEnabledFeatures = &deviceFeatures;
	createInfo.enabledExtensionCount = 0;
	

	if (enableValidationLayers)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else createInfo.enabledLayerCount = 0;
	
	if (vkCreateDevice(PhysicalDevice, &createInfo, nullptr, &Device) != VK_SUCCESS)
		throw std::runtime_error("Failed to create logical device!");
	
	vkGetDeviceQueue(Device, indices.graphicsFamily.value(), 0, &GraphicsQueue);
}*/

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
