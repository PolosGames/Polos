#pragma once

#include <GLFW/glfw3.h>

namespace polos
{
    void GLFWErrorCallback(int p_ErrorCode, char const* p_Description);

    void WindowCloseCallback(GLFWwindow* p_Window);

    void WindowFocusCallback(GLFWwindow* p_Window, int32 p_Focused);

    void WindowSizeCallback(GLFWwindow* p_Window, int32 p_Width, int32 p_Height);

    void WindowIconifyCallback(GLFWwindow* p_Window, int32 p_Iconified);

    void WindowMaximizeCallback(GLFWwindow* p_Window, int32 p_Maximized);

    void FramebufferSizeCallback(GLFWwindow* p_Window, int32 p_Width, int32 p_Height);

    void KeyCallback(
        GLFWwindow* p_Window,
        int32 p_Key,
        int32 p_Scancode,
        int32 p_Action,
        int32 p_Mods
    );

    void CharCallback(GLFWwindow* p_Window, uint32 unicode);

    void MouseButtonCallback(
        GLFWwindow* p_Window,
        int32 p_Button,
        int32 p_Action,
        int32 p_Mods
    );

    void ScrollCallback(
        GLFWwindow* p_Window,
        double p_XOffset,
        double p_YOffset
    );

    void CursorPosCallback(GLFWwindow* p_Window, double p_X, double p_Y);
} // namespace polos
