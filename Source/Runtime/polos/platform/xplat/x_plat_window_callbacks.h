#pragma once
#ifndef POLOS_PLATFORM_XPLAT_WINDOW_CALLBACKS_H_
#define POLOS_PLATFORM_XPLAT_WINDOW_CALLBACKS_H_

#include <GLFW/glfw3.h>

namespace polos
{
    void GLFWErrorCallback(int error_code, const char* description);

    void WindowCloseCallback(GLFWwindow* window);

    void WindowFocusCallback(GLFWwindow* window, int32 focused);

    void WindowSizeCallback(GLFWwindow* window, int32 width, int32 height);

    void WindowIconifyCallback(GLFWwindow* /*window*/, int32 iconified);

    void WindowMaximizeCallback(GLFWwindow* /*window*/, int32 maximized);

    void FramebufferSizeCallback(GLFWwindow* /*window*/, int32 width, int32 height);

    void KeyCallback(
        GLFWwindow* /*window*/,
        int32 key,
        int32 /*scancode*/,
        int32 action,
        int32 /*mods*/
    );

    void CharCallback(GLFWwindow*, uint32 unicode);

    void MouseButtonCallback(
        GLFWwindow* /*window*/,
        int32 button,
        int32 action,
        int32 /*mods*/
    );

    void ScrollCallback(
        GLFWwindow* /*window*/,
        double x_offset,
        double y_offset
    );

    void CursorPosCallback(GLFWwindow* /*window*/, double x, double y);
} // namespace polos

#endif// !POLOS_PLATFORM_XPLAT_WINDOW_CALLBACKS_H_
