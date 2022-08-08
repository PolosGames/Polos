#pragma once

#ifndef POLOS_WINDOW_XPLATWINDOW_H
#define POLOS_WINDOW_XPLATWINDOW_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include "polos/containers/containers.h"
#include "polos/core/i_window.h"
#include "polos/context/graphics_context.h"

namespace polos
{
    /// 
    /// Cross platform window that can be used with OpenGL and Vulkan
    /// 
    class XPlatWindow : public IWindow
    {
    public:
        void Initialize() override;
        void Shutdown()   override;
        void Destroy()    override;
    
        PL_NODISCARD int32 Width()  const override;
        PL_NODISCARD int32 Height() const override;
        PL_NODISCARD bool  Vsync()  const override;
        PL_NODISCARD bool  IsOpen() const override;
         
        void Vsync(bool vsync) override;
        void Update()          const override;
    private:
        friend class Application;
        static bool   m_IsInitialized;
        static uint32 m_WindowCount;
        
        GLFWwindow*   m_Window;
        UniquePtr<graphics_context> m_Context;
    };
}

#endif /* POLOS_WINDOW_XPLATWINDOW_H */