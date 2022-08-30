#pragma once

#ifndef POLOS_CONTEXT_GRAPHICSCONTEXT_H
#define POLOS_CONTEXT_GRAPHICSCONTEXT_H

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

#endif /* POLOS_CONTEXT_GRAPHICSCONTEXT_H */
