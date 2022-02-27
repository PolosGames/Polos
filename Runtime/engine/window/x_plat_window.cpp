#include "plpch.h"

#include "x_plat_window.h"
#if defined(USE_OPENGL) || defined(USE_VULKAN)

#include "events/window/window_events.h"
#include "events/input/input_events.h"

namespace polos
{
#if defined(USE_OPENGL) || defined(USE_VULKAN)
	Window* Window::NewWindow(const window_props& props)
	{
		return new XPlatWindow(props);
	}
#endif
	bool XPlatWindow::m_IsInitialized = false;
	uint32 XPlatWindow::m_WindowCount = 0;

	static void error_callback(int error_code, const char* description)
	{
		switch (error_code)
		{
		case GLFW_INVALID_ENUM: LOG_CORE_WARN("GLFW received an invalid enum to it's function! Desc: {0}", description); break;
		case GLFW_INVALID_VALUE: LOG_CORE_WARN("GLFW received an invalid value to it's function! Desc: {0}", description); break;
		case GLFW_OUT_OF_MEMORY: LOG_CORE_CRITICAL("A memory allocation failed within GLFW or the operating system! Desc: {0}", description); break;
		case GLFW_API_UNAVAILABLE: LOG_CORE_ERROR("GLFW could not find support for the requested API on the system! Desc: {0}", description); break;
		case GLFW_FORMAT_UNAVAILABLE: LOG_CORE_ERROR("The requested pixel format is not supported! Desc: {0}", description); break;
		}
	}

	void XPlatWindow::Shutdown()
	{
		glfwTerminate();
	}

	XPlatWindow::XPlatWindow(const window_props& props)
		: m_Props(props)
	{
		Initialize();
	}

	void XPlatWindow::Initialize()
	{
		if (!m_IsInitialized)
		{
			glfwSetErrorCallback(error_callback);
			int r = glfwInit();
			ASSERTSTR(r == GLFW_TRUE, "Failed to initialize GLFW!");
			m_IsInitialized = true;
		}

		GLFWmonitor* monitor = nullptr;
		const GLFWvidmode* mode = nullptr;
		if (m_Props.fullscreen)
		{
			monitor = glfwGetPrimaryMonitor();
			mode = glfwGetVideoMode(monitor);

			m_Props.width = mode->width;
			m_Props.height = mode->height;
			m_Props.refresh_rate = mode->refreshRate;
		}

		m_Window = glfwCreateWindow(m_Props.width, m_Props.height, m_Props.title.c_str(), monitor, nullptr);
		glfwSetWindowUserPointer(m_Window, &m_Props);
		glfwMakeContextCurrent(m_Window);

		m_Context = std::make_unique<graphics_context>();
		m_Context->Initialize(m_Window);

#pragma region window_events

		glfwSetWindowCloseCallback(m_Window,
			[](GLFWwindow* _)
			{
				EventBus::raise_event<window_close>();
			}
		);

		glfwSetWindowFocusCallback(m_Window,
			[](GLFWwindow* _, int32 focused)
			{
				EventBus::raise_event<window_focus>(focused);
			}
		);

		glfwSetWindowSizeCallback(m_Window,
			[](GLFWwindow* window, int32 width, int32 height)
			{
				window_props* props = static_cast<window_props*>(glfwGetWindowUserPointer(window));
				props->width = width;
				props->height = height;
				EventBus::raise_event<window_resize>(width, height);
			}
		);

		glfwSetWindowIconifyCallback(m_Window,
			[](GLFWwindow* _, int32 iconified)
			{
				EventBus::raise_event<window_iconify>(iconified);
			}
		);

		glfwSetWindowMaximizeCallback(m_Window,
			[](GLFWwindow* _, int32 maximized)
			{
				EventBus::raise_event<window_maximize>(maximized);
			}
		);

		glfwSetFramebufferSizeCallback(m_Window,
			[](GLFWwindow* _, int32 width, int32 height)
			{
				EventBus::raise_event<window_framebuffer_size>();
			}
		);

#pragma endregion

#pragma region key_input_events

		glfwSetKeyCallback(m_Window,
			[](GLFWwindow* _, int32 key, int32 scancode, int32 action, int32 mods)
			{
				switch (action)
				{
				case GLFW_PRESS:
					EventBus::raise_event<key_press>(key);
					break;
				case GLFW_RELEASE:
					EventBus::raise_event<key_release>(key);
					break;
				}
			}
		);

#pragma endregion

#pragma region mouse_input_events

		glfwSetMouseButtonCallback(m_Window,
			[](GLFWwindow* _, int32 button, int32 action, int32 mods)
			{
				switch (action)
				{
				case GLFW_PRESS:
					EventBus::raise_event<mouse_button_press>(button);
					break;
				case GLFW_RELEASE:
					EventBus::raise_event<mouse_button_release>(button);
					break;
				}
			}
		);

		glfwSetScrollCallback(m_Window,
			[](GLFWwindow* _, double x_offset, double y_offset)
			{
				EventBus::raise_event<mouse_scroll>((float)x_offset, (float)y_offset);
			}
		);

		glfwSetCursorPosCallback(m_Window,
			[](GLFWwindow* _, double x, double y)
			{
				EventBus::raise_event<mouse_move>((float)x, (float)y);
			}
		);

#pragma endregion
	}

	void XPlatWindow::Destroy()
	{
		glfwDestroyWindow(m_Window);
	}

	int32 XPlatWindow::Width() const
	{
		return m_Props.width;
	}

	int32 XPlatWindow::Height() const
	{
		return m_Props.height;
	}

	bool XPlatWindow::Vsync() const
	{
		return m_Props.vsync;
	}

	void XPlatWindow::Vsync(bool vsync)
	{
		glfwSwapInterval(vsync);
		m_Props.vsync = vsync;
	}

	void XPlatWindow::Update()
	{
		glfwPollEvents();
		glfwSwapBuffers(m_Window);
	}

	bool XPlatWindow::IsOpen() const
	{
		return !glfwWindowShouldClose(m_Window);
	}
} // namespace polos

#endif