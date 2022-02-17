#pragma once

#ifndef POLOS_WINDOW_XPLATWINDOW_H
#define POLOS_WINDOW_XPLATWINDOW_H

#include <GLFW/glfw3.h>

#include "window.h"
#include "context/graphics_context.h"

namespace polos
{
	/// 
	/// Cross platform window that can be used with OpenGL and Vulkan
	/// 
	class XPlatWindow : public Window
	{
	public:
		XPlatWindow(const window_props &props);
		~XPlatWindow() = default;

		void Initialize() override;
		void Shutdown() override;
		void Destroy() override;

		uint32 Width() override;
		uint32 Height() override;
		
		bool Vsync() override;
		void Vsync(bool vsync) override;
		
		void Update() override;
		bool IsOpen() override;
	private:
		GLFWwindow *window;

		static bool _is_glfw_initialized;
		static uint32 _glfw_window_count;

		window_props _props; // definitely make a global window prop variable
		std::unique_ptr<graphics_context> _context;
	};
}

#endif /* POLOS_WINDOW_XPLATWINDOW_H */