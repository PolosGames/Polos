///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_COMMUNICATION_INCLUDE_POLOS_COMMUNICATION_EVENT_BUS_HPP_
#define POLOS_COMMUNICATION_INCLUDE_POLOS_COMMUNICATION_EVENT_BUS_HPP_

#include "polos/communication/event.hpp"
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

    EventBus(const EventBus&) = delete;
    EventBus& operator=(const EventBus&) = delete;
    EventBus(EventBus&&) = delete;
    EventBus& operator=(EventBus&&) = delete;

    ///
    /// Get EventBus singleton
    ///
    /// @return Instance of EventBus
    static auto Instance() -> EventBus&;

    ///
    /// Subscribe to the specific event in the engine. Return type needs to be kept in order to unsubscribe.
    ///
    /// @todo make return type std::expected
    /// @tparam EventType Type of event to subscribe to (should be deduced)
    /// @param t_callback Functor that will receive the event
    /// @return Specific EventBus subscriber id.
    template<PolosEvent EventType>
    auto Subscribe(std::function<void(EventType&)> t_callback) -> std::int64_t;

    /// Trigger an event dispatch to all subscribers of the specific event.
    /// @tparam EventType
    /// @tparam Args
    /// @param args
    template<PolosEvent EventType, typename... Args>
    auto Dispatch(Args&&... args) -> std::size_t;
private:
    EventBus();

    // TODO: Make a freelist so we can unsubscribe
    using CallbackMap = std::pmr::unordered_map<std::int64_t, std::pmr::vector<BaseEventDelegate>>;

    polos::memory::DebugMemoryResource  m_allocator;
    std::int64_t                        m_next_id{0};
    CallbackMap                         m_callbacks;
    std::mutex                          m_mutex;
};

template<PolosEvent EventType>
auto EventBus::Subscribe(std::function<void(EventType&)> t_callback) -> std::int64_t
{
    LogTrace("[EventBus::Subscribe]");

    std::int64_t event_hash = EventHash<EventType>();
    LogDebug("EventHash: {}, Name: {}", event_hash, EventType::Name());

    std::lock_guard lock(m_mutex);

    // Give each subscriber of any event a unique id.
    auto subscriber_id = m_next_id++;

    m_callbacks.emplace(event_hash, std::pmr::vector<BaseEventDelegate>{m_allocator.GetMemoryResource()});
    auto& event_queue = m_callbacks[event_hash];
    event_queue.push_back(*std::launder(reinterpret_cast<BaseEventDelegate*>(&t_callback)));

    return subscriber_id;
}

template<PolosEvent EventType, typename... Args>
auto EventBus::Dispatch(Args&&... args) -> std::size_t
{
    LogTrace("[EventBus::Dispatch]");
    CallbackMap::iterator it = m_callbacks.find(EventHash<EventType>());
    if (it == m_callbacks.end())
    {
        LogDebug("[EventBus::Dispatch] No subscribers found for type {}", EventType::Name());
        return 0U;
    }

    auto& subcribers_callbacks = it->second;

    for (auto const& subscriber : subcribers_callbacks)
    {
        EventType event(std::forward<Args>(args)...);
        std::invoke(subscriber, event);
    }
    LogDebug("Dispatched event of type {{{}}} to {} subscribers...", EventType::Name(), subcribers_callbacks.size());

    return subcribers_callbacks.size();
}

template<PolosEvent EventType>
auto Subscribe(std::function<void(EventType&)> t_callback) -> std::int64_t
{
    return EventBus::Instance().Subscribe(t_callback);
}

template<PolosEvent EventType, typename... Args>
auto Dispatch(Args&&... args) -> std::size_t
{
    return EventBus::Instance().Dispatch<EventType>(std::forward<Args>(args)...);
}

}// namespace polos::communication

#endif// POLOS_COMMUNICATION_INCLUDE_POLOS_COMMUNICATION_EVENT_BUS_HPP_
