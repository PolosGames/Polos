#include "polos/polos_pch.h"
#ifdef USE_OPENGL

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "polos/utils/feature.h"

#include "polos/context/graphics_context.h"

#define PL_FILTER_OPENGL_NOTIFICATIONS 1

namespace polos
{
#ifdef PL_DEBUG
    static void DebugCallback(GLenum source, GLenum type, GLuint /*id*/, GLenum severity, GLsizei /*length*/, GLchar const* message, void const* /*user_param*/)
    {
        auto const message_source = [source] {
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
    
        auto const message_type = [type] {
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
        
        LOG_ENGINE_TRACE("OpenGL Context");
        switch (severity)
        {
        case GL_DEBUG_SEVERITY_HIGH:
            LOG_ENGINE_CRITICAL("{0} on {1},\n{2}", message_type, message_source, message);
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            LOG_ENGINE_ERROR("{0} on {1},\n{2}", message_type, message_source, message);
            break;
        case GL_DEBUG_SEVERITY_LOW:
            LOG_ENGINE_WARN("{0} on {1},\n{2}", message_type, message_source, message);
            break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            LOG_ENGINE_INFO("Notification: {0}, Source: {1},\n{2}", message_type, message_source, message);
            break;
        }
    }
#endif /* PL_DEBUG */

    bool GraphicsContext::s_IsInitialized;

    void GraphicsContext::Initialize(void* window_handle)
    {
        m_Window = window_handle;
        
        int r = gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
        static_cast<void>(r);
        ASSERTSTR(r, "Failed to load OpenGL context!");

        int w;
        int h;
        glfwGetFramebufferSize(static_cast<GLFWwindow*>(m_Window), &w, &h);
        glViewport(0, 0, w, h);

#ifdef PL_DEBUG
//        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

        glDebugMessageCallback(DebugCallback, nullptr);
        /*
        *    TODO: Filter some of the unnecessary messages here
        *    https://www.khronos.org/opengl/wiki/Debug_Output#Message_filtering
        *    glDebugMessageControl();
        */
#if PL_FILTER_OPENGL_NOTIFICATIONS
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
#endif // !PL_FILTER_OPENGL_NOTIFICATIONS
        
#endif // !PL_DEBUG
        if (!s_IsInitialized)
        {
            s_IsInitialized = true;
        }

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
}

#endif