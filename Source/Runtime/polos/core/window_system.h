#pragma once

#include "polos/core/window.h"
#include "polos/containers/containers.h"
#include "polos/containers/delegate.h"
#include "polos/graphics/graphics_context.h"
#include "polos/core/events/window/window_close.h"
#include "polos/utils/guid.h"

namespace polos
{
    /// Implementation inside the platform folder of the specific platform.
    class WindowSystem
    {
    public:
        void Startup();
        void Shutdown();

        PL_NODISCARD static polos::GUID NewWindow(window_props& p_NewProps);
        static void SwitchWindow(polos::GUID p_Guid);

        // This is also the main window for the current app. Whether it is the
        // editor or the game itself.
        PL_NODISCARD static polos::GUID            GetAppWindowGUID();
        PL_NODISCARD static Optional<window_props> GetAppWindowProps();
        PL_NODISCARD static void*                  GetAppWindowHandle();

        PL_NODISCARD static Optional<window_props> GetWindowProps(polos::GUID p_WindowGUID);
        PL_NODISCARD static void*                  GetWindowHandle(polos::GUID p_WindowGUID);
        
        PL_NODISCARD static bool IsInitialized();

        static void Update();
    private:
        void                 on_window_close(window_close& p_Event);
        static std::size_t   find_index_with_guid(polos::GUID p_WindowGUID);
    private:
        static WindowSystem* s_Instance;

        bool                       m_IsInitialized{};
        DArray<polos::GUID>        m_WinGUIDs;
        DArray<window_props>       m_WinProps;
        DArray<void*>              m_WinHandles;
        SharedPtr<GraphicsContext> m_GfxContext;
    };
} // namespace polos
