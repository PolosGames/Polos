#include "plpch.h"

#ifdef USE_OPENGL

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "context/graphics_context.h"

namespace polos
{
	void graphics_context::Initialize(void *window_handle)
	{
		_window = window_handle;
		GLFWwindow *gwindow = reinterpret_cast<GLFWwindow *>(_window);
		
		int r = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		ASSERT_S(r, "Failed to load OpenGL context!");

		is_initialized = true;
	}
}

#endif