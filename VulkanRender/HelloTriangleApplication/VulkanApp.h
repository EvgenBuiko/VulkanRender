#pragma once
#include <optional>
#include "vulkan/vulkan.h"
#include <vector>

struct QueueFamilyIndices
{
	std::optional<uint32_t> graphicsFamily;
	
	bool isComplete() { return graphicsFamily.has_value(); }
};

class VulkanApp
{
public:
	VulkanApp();
	virtual ~VulkanApp();

	virtual std::vector<const char*> GetRequiredExtensions() = 0;
	virtual const char* GetApplicationName() = 0;

	void InitVulkan();
	void CleanupVulkan();

private:
	VkResult createVkInstance();
	bool checkValidationLayerSupport();
	void setupDebugMessenger();
	void pickPhysicalDevice();
	int rateDeviceSuitability(VkPhysicalDevice device);
	QueueFamilyIndices findQueueFamillies(VkPhysicalDevice device);
	bool isDeviceSuitable(VkPhysicalDevice device);
	void createLogicalDevice();

public:
// Data
	VkInstance Instance;
	VkDevice Device;
	VkQueue GraphicsQueue;
	VkDebugUtilsMessengerEXT DebugMessenger;
	VkPhysicalDevice PhysicalDevice;

#ifdef _DEBUG
	const bool enableValidationLayers = true;
#else
	const bool enableValidationLayers = false;
#endif // _DEBUG

// configuration variables
	const std::vector<const char*> validationLayers = 
	{
		"VK_LAYER_KHRONOS_validation"
	};

};

