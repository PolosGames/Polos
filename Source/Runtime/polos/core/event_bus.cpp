#include "polos/polos_pch.h"

#include "event_bus.h"

namespace polos
{
    EventBus* EventBus::s_Instance;

    void EventBus::Startup()
    {
        s_Instance = this;
    }

    void EventBus::Shutdown()
    {
        s_Instance = nullptr;
    }
}