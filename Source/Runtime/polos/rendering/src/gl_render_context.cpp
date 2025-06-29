///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/rendering/gl_render_context.hpp"

#include "polos/logging/log_macros.hpp"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <cstdint>

namespace polos::rendering
{

#ifndef NDEBUG
namespace
{

static void DebugCallback(GLenum t_source, GLenum t_type, GLuint /*id*/, GLenum t_severity, GLsizei /*length*/,
                          GLchar const* t_message, void const* /*user_param*/)
{
    auto const message_source = [t_source]
    {
        switch (t_source)
        {
            case GL_DEBUG_SOURCE_API: return "OpenGL API";
            case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "Window System";
            case GL_DEBUG_SOURCE_SHADER_COMPILER: return "Shader Compiler";
            case GL_DEBUG_SOURCE_THIRD_PARTY: return "Third Party";
            case GL_DEBUG_SOURCE_APPLICATION: return "User Created App";
            default: return "Unknown";
        }
    }();

    auto const message_type = [t_type]
    {
        switch (t_type)
        {
            case GL_DEBUG_TYPE_ERROR: return "Error";
            case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "Deprecated Behaviour";
            case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "Undefined Behaviour";
            case GL_DEBUG_TYPE_PORTABILITY: return "Portability Issue";
            case GL_DEBUG_TYPE_PERFORMANCE: return "Performance Issue";
            case GL_DEBUG_TYPE_MARKER: return "Marker";
            case GL_DEBUG_TYPE_PUSH_GROUP: return "Push Group";
            case GL_DEBUG_TYPE_POP_GROUP: return "Pop Group";
            default: return "Unknown";
        }
    }();

    LogTrace("OpenGL Context");
    switch (t_severity)
    {
        case GL_DEBUG_SEVERITY_HIGH: LogCritical("{0} on {1},\n{2}", message_type, message_source, t_message); break;
        case GL_DEBUG_SEVERITY_MEDIUM: LogError("{0} on {1},\n{2}", message_type, message_source, t_message); break;
        case GL_DEBUG_SEVERITY_LOW: LogWarn("{0} on {1},\n{2}", message_type, message_source, t_message); break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            LogInfo("Notification: {0}, Source: {1},\n{2}", message_type, message_source, t_message);
            break;
    }
}

}// namespace

#endif// !NDEBUG

bool InitializeRenderContext()
{
    std::int32_t result = gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
    if (!result)
    {
        LogCritical("GL COULD NOT BE INITIALIZED!");
        return false;
    }

#ifndef NDEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

    glDebugMessageCallback(DebugCallback, nullptr);

    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
#endif// !NDEBUG


    return true;
}

}// namespace polos::rendering
