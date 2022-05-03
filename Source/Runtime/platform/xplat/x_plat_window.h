#pragma once

#ifndef POLOS_WINDOW_XPLATWINDOW_H
#define POLOS_WINDOW_XPLATWINDOW_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include "core/i_window.h"
#include "renderer/context/graphics_context.h"

namespace polos
{
    /// 
    /// Cross platform window that can be used with OpenGL and Vulkan
    /// 
    class XPlatWindow : public IWindow
    {
    public:
        explicit XPlatWindow(const window_props &props);

        void Initialize() override;
        void Shutdown() override;
        void Destroy() override;
    
        PL_NODISCARD int32 Width() const override;
        PL_NODISCARD int32 Height() const override;
        
        PL_NODISCARD bool Vsync() const override;
        void Vsync(bool vsync) override;
        
        void Update() override;
        PL_NODISCARD bool IsOpen() const override;
    private:
        friend class Application;
        GLFWwindow*   m_Window;

        static bool   m_IsInitialized;
        static uint32 m_WindowCount;

        window_props  m_Props; // definitely make a global window prop variable
        std::unique_ptr<graphics_context> m_Context;
    };
}

#endif /* POLOS_WINDOW_XPLATWINDOW_H */