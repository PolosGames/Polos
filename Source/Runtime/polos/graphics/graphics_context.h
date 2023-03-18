#pragma once

namespace polos
{
    class GraphicsContext
    {
    public:
        void Initialize(void* p_WindowHandle);
    public:
        static bool s_IsInitialized;
    private:
        void* m_Window;
    };
}
