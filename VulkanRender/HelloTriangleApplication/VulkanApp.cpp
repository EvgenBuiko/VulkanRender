#include "VulkanApp.h"
#include <stdexcept>
#include <iostream>
#include <map>

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
	void* userData)
{
	if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
		std::cerr << "Validation layer: " << callbackData->pMessage << std::endl;
	return VK_FALSE;
}

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance,
	const VkDebugUtilsMessengerCreateInfoEXT* createInfo,
	const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pCallback)
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
}


VulkanApp::VulkanApp()
{
	Instance = NULL;
	DebugMessenger = NULL;
	PhysicalDevice = VK_NULL_HANDLE;
}

VulkanApp::~VulkanApp()
{

}

void VulkanApp::InitVulkan()
{
	if (createVkInstance() != VK_SUCCESS)
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

	setupDebugMessenger();
	pickPhysicalDevice();
	createLogicalDevice();
	// ...
}

void VulkanApp::CleanupVulkan()
{
	if (enableValidationLayers)
	{
		DestroyDebugUtilsMessenger(Instance, DebugMessenger, nullptr);
	}
	vkDestroyInstance(Instance, nullptr);
	vkDestroyDevice(Device, nullptr);
}

VkResult VulkanApp::createVkInstance()
{
	if (enableValidationLayers && !checkValidationLayerSupport())
		throw std::runtime_error("Validation layers enabled but not available");

	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = GetApplicationName();
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Example";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	auto extentions = GetRequiredExtensions();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extentions.size());
	createInfo.ppEnabledExtensionNames = extentions.data();

	if (enableValidationLayers)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else createInfo.enabledLayerCount = 0;

	return vkCreateInstance(&createInfo, nullptr, &Instance);
}

bool VulkanApp::checkValidationLayerSupport()
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

void VulkanApp::setupDebugMessenger()
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

void VulkanApp::pickPhysicalDevice()
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

	if (PhysicalDevice == VK_NULL_HANDLE)
		throw std::runtime_error("Failed to find suitable GPU");
}

int VulkanApp::rateDeviceSuitability(VkPhysicalDevice device)
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

QueueFamilyIndices VulkanApp::findQueueFamillies(VkPhysicalDevice device)
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

bool VulkanApp::isDeviceSuitable(VkPhysicalDevice device)
{
	QueueFamilyIndices indices = findQueueFamillies(device);
	return indices.isComplete();
}

void VulkanApp::createLogicalDevice()
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

}
