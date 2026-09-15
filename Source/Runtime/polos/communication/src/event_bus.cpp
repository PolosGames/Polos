///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/communication/event_bus.hpp"

#include <vector>

namespace polos::communication
{

EventBus::EventBus() = default;

auto EventBus::Instance() -> EventBus&
{
    static EventBus sInstance;
    return sInstance;
}

EventBus::~EventBus() = default;

auto EventBus::DispatchDeferredEvents() -> void
{
    for (auto& event_ptr : mDeferredEvents)
    {
        std::int64_t const event_hash = event_ptr->Hash();
        auto const         iter       = mCallbacks.find(event_hash);
        if (iter != mCallbacks.end())
        {
            auto& subscribers_callbacks = iter->second;
            for (auto& callback : subscribers_callbacks) { callback(*event_ptr); }
        }
    }
    mDeferredEvents.clear();
}

auto DispatchDeferredEvents() -> void
{
    EventBus::Instance().DispatchDeferredEvents();
}

}// namespace polos::communication
