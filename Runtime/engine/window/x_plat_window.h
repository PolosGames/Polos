#pragma once

#ifndef POLOS_WINDOW_XPLATWINDOW_H
#define POLOS_WINDOW_XPLATWINDOW_H

#include <memory>

#include <GLFW/glfw3.h>

#include "core/window.h"
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

		void Initialize() override;
		void Shutdown() override;
		void Destroy() override;

		int32 Width() const override;
		int32 Height() const override;
		
		bool Vsync() const override;
		void Vsync(bool vsync) override;
		
		void Update() override;
		bool IsOpen() const override;
	private:
		GLFWwindow*   m_Window;

		static bool   m_IsInitialized;
		static uint32 m_WindowCount;

		window_props  m_Props; // definitely make a global window prop variable
		std::unique_ptr<graphics_context> m_Context;
	};
}

#endif /* POLOS_WINDOW_XPLATWINDOW_H */