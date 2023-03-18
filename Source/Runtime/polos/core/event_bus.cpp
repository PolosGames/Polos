
#include "event_bus.h"

#include "polos/core/engine/engine.h"

namespace polos
{
    EventBus* EventBus::s_Instance;

    EventBus::EventBus()
    {
        SUBSCRIBE_TO_ENGINE_STARTUP(Startup);
        SUBSCRIBE_TO_ENGINE_SHUTDOWN(Shutdown);
    }

    void EventBus::Startup()
    {
        s_Instance = this;
    }

    void EventBus::Shutdown()
    {
        s_Instance = nullptr;
    }
}