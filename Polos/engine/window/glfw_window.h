#if defined(USE_OPENGL) || defined(USE_VULKAN)

#pragma once

#ifndef POLOS_WINDOW_GLFWWINDOW_H
#define POLOS_WINDOW_GLFWWINDOW_H

#include "core/window.h"
#include "GLFW/glfw3.h"

namespace polos
{
	class glfw_window : public window
	{
	public:
		glfw_window(const window_props&);
		~glfw_window() = default;

		void initialize() override;
		void destroy() override;

		uint32 width() override;
		uint32 height() override;
		
		bool vsync() override;
		void vsync(bool vsync) override;
		
		void update() override;
		bool is_open() override;
	private:
		GLFWwindow *window;

		static bool _is_glfw_initialized;
		static uint32 _glfw_window_count;

		window_props props; // definitely make a global window prop variable
	};
}

#endif /* POLOS_WINDOW_GLFWWINDOW_H */

#endif