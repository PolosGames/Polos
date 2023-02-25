#pragma once

namespace polos
{
    class GraphicsContext
    {
    public:
        void Initialize(void* window_handle);
    public:
        static bool s_IsInitialized;
    private:
        void* m_Window;
    };
}
