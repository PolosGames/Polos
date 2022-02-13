#if defined(USE_OPENGL) || defined(USE_VULKAN)

#pragma once

#ifndef POLOS_WINDOW_GLFWWINDOW_H
#define POLOS_WINDOW_GLFWWINDOW_H

#include <GLFW/glfw3.h>

#include "window.h"
#include "context/graphics_context.h"

namespace polos
{
	class glfw_window : public Window
	{
	public:
		glfw_window(const WindowProps&);
		~glfw_window() = default;

		void initialize() override;
		void shutdown() override;
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

		WindowProps _props; // definitely make a global window prop variable
		std::unique_ptr<graphics_context> _context;
	};
}

#endif /* POLOS_WINDOW_GLFWWINDOW_H */

#endif