#include "plpch.h"

#ifdef USE_OPENGL

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "context/graphics_context.h"
#include "utils/alias.h"

namespace polos
{
	static void DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const* message, void const* userParam)
	{
		std::string message_source;
		std::string message_type;
		std::string message_severity;

		switch (source)
		{
		case GL_DEBUG_SOURCE_API:             message_source = "OpenGL API";      break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   message_source = "Window System";   break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER: message_source = "Shader Compiler"; break;
		case GL_DEBUG_SOURCE_THIRD_PARTY:     message_source = "Third Party";     break;
		case GL_DEBUG_SOURCE_APPLICATION:     message_source = "User Created App"; break;
		case GL_DEBUG_SOURCE_OTHER:           message_source = "Unknown";         break;
		}

		switch (type)
		{
		case GL_DEBUG_TYPE_ERROR:				message_type = "Error";					break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:	message_type = "Deprecated Behaviour";	break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:	message_type = "Undefined Behaviour";	break;
		case GL_DEBUG_TYPE_PORTABILITY:			message_type = "Portability Issue";		break;
		case GL_DEBUG_TYPE_PERFORMANCE:			message_type = "Performance Issue";		break;
		case GL_DEBUG_TYPE_MARKER:				message_type = "Marker";				break;
		case GL_DEBUG_TYPE_PUSH_GROUP:			message_type = "Push Group";			break;
		case GL_DEBUG_TYPE_POP_GROUP:			message_type = "Pop Group";				break;
		case GL_DEBUG_TYPE_OTHER:				message_type = "Unknown";				break;
		}

		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH:
			LOG_CORE_CRITICAL("Critical {0} on {1}", message_type, message_source);
			break;
		case GL_DEBUG_SEVERITY_MEDIUM:
			LOG_CORE_ERROR("Error {0} on {1}", message_type, message_source);
			break;
		case GL_DEBUG_SEVERITY_LOW:
			LOG_CORE_WARN("Warn {0} on {1}", message_type, message_source);
			break;
		case GL_DEBUG_SEVERITY_NOTIFICATION:
			LOG_CORE_INFO("Type: {0} , Source: {1}", message_type, message_source);
			break;
		}
	}

	void graphics_context::Initialize(void *window_handle)
	{
		m_window = window_handle;
		GLFWwindow *gwindow = reinterpret_cast<GLFWwindow *>(m_window);
		
		int r = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		ASSERTSTR(r, "Failed to load OpenGL context!");

#ifdef PL_DEBUG
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

		glDebugMessageCallback(DebugCallback, nullptr);
#endif

		is_initialized = true;
	}
}

#endif