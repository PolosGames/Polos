///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/communication/event_bus.hpp"

#include <mutex>
#include <unordered_map>
#include <vector>

namespace polos::communication
{

EventBus::EventBus()
    : m_allocator{"EventBus"},
      m_callbacks{m_allocator.GetMemoryResource()}
{}

auto EventBus::Instance() -> EventBus&
{
    static EventBus instance;
    return instance;
}

EventBus::~EventBus() = default;

auto EventBus::DispatchDeferredEvents() -> void
{
    for (auto& event_ptr : m_deferred_events)
    {
        std::int64_t          event_hash = event_ptr->Hash();
        CallbackMap::iterator it         = m_callbacks.find(event_hash);
        if (it != m_callbacks.end())
        {
            auto& subscribers_callbacks = it->second;
            for (auto& callback : subscribers_callbacks) { callback(*event_ptr); }
        }
    }
    m_deferred_events.clear();
}

auto DispatchDeferredEvents() -> void
{
    EventBus::Instance().DispatchDeferredEvents();
}

}// namespace polos::communication
