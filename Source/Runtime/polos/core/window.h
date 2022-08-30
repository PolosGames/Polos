#pragma once

#ifndef POLOS_CORE_WINDOW_H
#define POLOS_CORE_WINDOW_H

#include "polos/utils/alias.h"
#include "polos/utils/feature.h"
#include "polos/context/graphics_context.h"

namespace polos
{
    struct window_props
    {
        std::string title  = "Polos";
        int32 width        = 1280;
        int32 height       = 720;
        int32 refresh_rate = 60;
        bool vsync         = true;
        bool fullscreen    = false;
    private:
        char m_holder[2] = {44, 34};
    };

    class Window
    {
    public:
        Window() = default;
        virtual ~Window() = default;

        virtual void Create();
        virtual void Destroy();

        PL_NODISCARD virtual int32 Width() const;
        PL_NODISCARD virtual int32 Height() const;
        PL_NODISCARD virtual bool  Vsync() const;
        PL_NODISCARD virtual bool  IsOpen() const;
        
        virtual void Vsync(bool vsync);

        virtual void Update() const;

    public:
        window_props props;
    private:
        friend class WindowSystem;

        bool                     m_IsMainWindow;
        WeakPtr<GraphicsContext> m_GfxContext;
        void*                    m_WinHandle; 
    };
}

#endif /* POLOS_CORE_WINDOW_H */
