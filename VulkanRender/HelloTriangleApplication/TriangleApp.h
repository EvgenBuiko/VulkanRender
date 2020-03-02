#pragma once
#include <vulkan/vulkan.h>

#include <iostream>
#include <stdexcept>
#include <functional>
#include <cstdlib>

class TriangleApp
{
public:
	void run()
	{
		initVulkan();
		mainLoop();
		cleanup();
	}

private:
	void initVulkan();
	void mainLoop();
	void cleanup();
};

