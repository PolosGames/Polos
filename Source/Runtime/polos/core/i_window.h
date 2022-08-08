#pragma once

#ifndef POLOS_CORE_WINDOW_H
#define POLOS_CORE_WINDOW_H

#include "polos/utils/alias.h"
#include "polos/utils/feature.h"

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

    class IWindow
    {
    public:
        IWindow() = default;
        virtual ~IWindow() = default;

        virtual void Initialize() = 0;
        virtual void Shutdown() = 0;
        virtual void Destroy() = 0;

        PL_NODISCARD virtual int32 Width() const = 0;
        PL_NODISCARD virtual int32 Height() const = 0;
        PL_NODISCARD virtual bool  Vsync() const = 0;
        PL_NODISCARD virtual bool  IsOpen() const = 0;
        
        virtual void Vsync(bool vsync) = 0;

        virtual void Update() const = 0;

        window_props props;
    };
}

#endif /* POLOS_CORE_WINDOW_H */
