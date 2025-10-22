///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_COMMUNICATION_INCLUDE_POLOS_COMMUNICATION_EVENT_BUS_HPP_
#define POLOS_COMMUNICATION_INCLUDE_POLOS_COMMUNICATION_EVENT_BUS_HPP_

#include "polos/communication/event.hpp"
#include "polos/communication/event_defer_options.hpp"
#include "polos/communication/module_macros.hpp"
#include "polos/logging/log_macros.hpp"
#include "polos/memory/debug_memory_resource.hpp"

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <expected>
#include <functional>
#include <utility>

namespace polos::communication
{
template<class T>
concept PolosEvent = std::derived_from<std::remove_cvref_t<T>, base_event>;

/// Simple event bus for triggering subscriber functors for a specific event.
///
/// The subscriber functors should not hang.
/// The type of the functor shall always conform to the format of void(EventType&).
/// Helper functions for not manually triggering Instance of this class is created. When subscribing, unsubscribing, or
/// dispatching, the functions communication::Subscribe, communication::Unsubscribe, communication::Dispatch can be used
/// instead of using EventBus verbosely.
class COMMUNICATION_EXPORT EventBus
{
    using BaseEventDelegate = std::function<void(base_event&)>;
public:
    ~EventBus();

    EventBus(const EventBus&)            = delete;
    EventBus& operator=(const EventBus&) = delete;
    EventBus(EventBus&&)                 = delete;
    EventBus& operator=(EventBus&&)      = delete;

    ///
    /// @brief Get EventBus singleton
    ///
    /// @return Instance of EventBus
    static auto Instance() -> EventBus&;

    ///
    /// @brief Subscribe to the specific event in the engine. Return type needs to be kept in order to unsubscribe.
    ///
    /// @todo make return type std::expected
    /// @tparam EventType Type of event to subscribe to (should be deduced)
    /// @param t_callback Functor that will receive the event
    /// @return Specific EventBus subscriber id.
    template<PolosEvent EventType>
    auto Subscribe(std::function<void(EventType&)> t_callback) -> std::int64_t;

    /// @brief Trigger an event dispatch to all subscribers of the specific event.
    /// @tparam EventType
    /// @tparam Args
    /// @param args
    template<PolosEvent EventType, typename... Args>
    auto Dispatch(EventDeferOptions const t_defer, Args&&... t_args) -> std::size_t;

    /// @brief Dispatches all deferred events that were queued with EventDeferOptions::NextFrame.
    /// This should be called at the beginning of each frame.
    auto DispatchDeferredEvents() -> void;
private:
    EventBus();

    // TODO: Make a freelist so we can unsubscribe
    using CallbackMap = std::unordered_map<std::int64_t, std::vector<BaseEventDelegate>>;

    std::int64_t                             m_next_id{0};
    CallbackMap                              m_callbacks;
    std::vector<std::unique_ptr<base_event>> m_deferred_events;
};

template<PolosEvent EventType>
auto EventBus::Subscribe(std::function<void(EventType&)> t_callback) -> std::int64_t
{
    LogTraceCtx(LOG_CTX_POLOS, "[EventBus::Subscribe]");

    std::int64_t event_hash = EventHash<EventType>();
    LogDebugCtx(LOG_CTX_POLOS, "Subscribing to: EventHash: {}, Name: {}", event_hash, EventType::Name());

    // Give each subscriber of any event a unique id.
    auto subscriber_id = m_next_id++;

    m_callbacks.insert({event_hash, std::vector<BaseEventDelegate>()});
    auto& event_queue = m_callbacks[event_hash];
    event_queue.push_back(*std::launder(reinterpret_cast<BaseEventDelegate*>(&t_callback)));

    return subscriber_id;
}

template<PolosEvent EventType, typename... Args>
auto EventBus::Dispatch(EventDeferOptions const t_defer, Args&&... t_args) -> std::size_t
{
    CallbackMap::iterator it = m_callbacks.find(EventHash<EventType>());
    if (it == m_callbacks.end())
    {
        LogTraceCtx(LOG_CTX_POLOS, "[EventBus::Dispatch] No subscribers found for type {}", EventType::Name());
        return 0U;
    }

    auto& subcribers_callbacks = it->second;

    if (t_defer == EventDeferOptions::kImmediate)
    {
        EventType event{std::forward<Args>(t_args)...};

        for (auto& callback : subcribers_callbacks) { callback(event); }
    }
    else if (t_defer == EventDeferOptions::kNextFrame)
    {
        m_deferred_events.push_back(std::make_unique<EventType>(std::forward<Args>(t_args)...));
    }

    return subcribers_callbacks.size();
}

template<PolosEvent EventType>
auto Subscribe(std::function<void(EventType&)> t_callback) -> std::int64_t
{
    return EventBus::Instance().Subscribe(t_callback);
}

template<PolosEvent EventType, typename... Args>
auto DispatchNow(Args&&... args) -> std::size_t
{
    return EventBus::Instance().Dispatch<EventType>(EventDeferOptions::kImmediate, std::forward<Args>(args)...);
}

template<PolosEvent EventType, typename... Args>
auto DispatchDefer(Args&&... args) -> std::size_t
{
    return EventBus::Instance().Dispatch<EventType>(EventDeferOptions::kNextFrame, std::forward<Args>(args)...);
}

COMMUNICATION_EXPORT auto DispatchDeferredEvents() -> void;

}// namespace polos::communication

#endif// POLOS_COMMUNICATION_INCLUDE_POLOS_COMMUNICATION_EVENT_BUS_HPP_
