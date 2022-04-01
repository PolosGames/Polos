#include "polos_pch.h"
#ifdef USE_OPENGL

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "log.h"
#include "utils/feature.h"

#include "context/graphics_context.h"

namespace polos
{
    static void DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const* message, void const* user_param)
    {
        const std::string message_source = [source] {
            switch (source)
            {
                case GL_DEBUG_SOURCE_API:             return "OpenGL API";
                case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   return "Window System";
                case GL_DEBUG_SOURCE_SHADER_COMPILER: return "Shader Compiler";
                case GL_DEBUG_SOURCE_THIRD_PARTY:     return "Third Party";
                case GL_DEBUG_SOURCE_APPLICATION:     return "User Created App";
                default:                              return "Unknown";
            }
        }();
        
        const std::string message_type = [type] {
            switch (type)
            {
                case GL_DEBUG_TYPE_ERROR:				return "Error";
                case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:	return "Deprecated Behaviour";
                case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:	return "Undefined Behaviour";
                case GL_DEBUG_TYPE_PORTABILITY:			return "Portability Issue";
                case GL_DEBUG_TYPE_PERFORMANCE:			return "Performance Issue";
                case GL_DEBUG_TYPE_MARKER:				return "Marker";
                case GL_DEBUG_TYPE_PUSH_GROUP:			return "Push Group";
                case GL_DEBUG_TYPE_POP_GROUP:			return "Pop Group";
                default:                                return "Unknown";
            }
        }();

        switch (severity)
        {
        case GL_DEBUG_SEVERITY_HIGH:
            LOG_CORE_CRITICAL("Critical {0} on {1}, {2}", message_type, message_source, message);
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            LOG_CORE_ERROR("Error {0} on {1}, {2}", message_type, message_source, message);
            break;
        case GL_DEBUG_SEVERITY_LOW:
            LOG_CORE_WARN("Warn {0} on {1}, {2}", message_type, message_source, message);
            break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            LOG_CORE_INFO("Type: {0} , Source: {1}", message_type, message_source, message);
            break;
        }
    }

    void graphics_context::Initialize(void* window_handle)
    {
        m_window = window_handle;
        PL_MAYBEUNUSED auto* window = reinterpret_cast<GLFWwindow*>(m_window);
        
        int r = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
        ASSERTSTR(r, "Failed to load OpenGL context!");

#ifdef PL_DEBUG
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

        glDebugMessageCallback(DebugCallback, nullptr);

        // TODO: Filter some of the unnecessary messages here
        // https://www.khronos.org/opengl/wiki/Debug_Output#Message_filtering
        // glDebugMessageControl();
#endif

        is_initialized = true;
    }
}

#endif