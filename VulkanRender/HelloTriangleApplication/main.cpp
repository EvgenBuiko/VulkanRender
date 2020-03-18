#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#pragma once

#include "TriangleApp.h"

#include <iostream>
#include <stdexcept>
#include <functional>
#include <cstdlib>


int main()
{
	TriangleApp app;
	try { app.run(); }
	catch (const std::exception & e)
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}