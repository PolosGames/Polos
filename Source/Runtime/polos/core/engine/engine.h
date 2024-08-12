#pragma once

#include "polos/containers/delegate.h"

namespace polos
{
    class Engine
    {
        using Subscriber = void(*)(void);
    public:
        static void Run();

        static void SubscribeToStartup(Delegate<void()> p_Delegate);
        static void SubscribeToShutdown(Delegate<void()> p_Delegate);
    private:
        static Engine* s_Instance;

        std::vector<Delegate<void()>> m_StartupSequence;
        std::vector<Delegate<void()>> m_ShutdownSequnce;
    };
} // namespace polos

#define SUBSCRIBE_TO_ENGINE_STARTUP(Func)                   \
    ::polos::Engine::SubscribeToStartup(                    \
        ::polos::Delegate<void()>::template From< \
            std::remove_cvref_t<decltype(*this)>,           \
            &std::remove_cvref_t<decltype(*this)>::Func>(this))

#define SUBSCRIBE_TO_ENGINE_SHUTDOWN(Func)                  \
    ::polos::Engine::SubscribeToShutdown(                   \
        ::polos::Delegate<void()>::template From< \
            std::remove_cvref_t<decltype(*this)>,           \
            &std::remove_cvref_t<decltype(*this)>::Func>(this))
