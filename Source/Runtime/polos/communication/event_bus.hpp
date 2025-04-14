//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef CORE_COMMUNICATION_EVENT_BUS_H
#define CORE_COMMUNICATION_EVENT_BUS_H

#include "polos/communication/event.hpp"

#include "polos/communication/module_macros.hpp"
#include "polos/logging/logging_macros.hpp"

#include <concepts>
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
    static EventBus& Instance();

    ///
    /// Subscribe to the specific event in the engine. Return type needs to be kept in order to unsubscribe.
    ///
    /// @todo make return type std::expected
    /// @tparam EventType Type of event to subscribe to (should be deduced)
    /// @param t_callback Functor that will receive the event
    /// @return Specific EventBus subscriber id.
    template<PolosEvent EventType>
    [[nodiscard]] std::int64_t Subscribe(std::function<void(EventType&)> t_callback);

    ///
    /// @tparam EventType Type of event to unsubscribe from (should be deduced)
    /// @param t_sub_id Subscriber id which is the output of Subscribe(cb)
    /// @return true if operation is successful, false if not
    template<PolosEvent EventType>
    bool Unsubscribe(std::int64_t t_sub_id) const;

    /// Trigger an event dispatch to all subscribers of the specific event.
    /// @tparam EventType
    /// @tparam Args
    /// @param args
    template<PolosEvent EventType, typename... Args>
    void Dispatch(Args&&... args);
private:
    EventBus();

    std::int64_t subscribe_internal(std::int64_t t_type_hash, std::function<void(base_event&)> const& t_callback) const;
    bool         unsubscribe_internal(std::int64_t t_type_hash, std::int64_t t_sub_id) const;
    std::pair<BaseEventDelegate const*, std::size_t> retrieve_subscribers(std::int64_t t_type_hash) const;

    class Impl;
    Impl* m_impl;
};

template <PolosEvent EventType>
std::int64_t EventBus::Subscribe(std::function<void(EventType&)> t_callback)
{
    return subscribe_internal(EventHash<EventType>(), *std::launder(reinterpret_cast<BaseEventDelegate*>(&t_callback)));
}

template<PolosEvent EventType> bool EventBus::Unsubscribe(std::int64_t const t_sub_id) const
{
    return unsubscribe_internal(EventHash<EventType>(), t_sub_id);
}

template<PolosEvent EventType, typename... Args>
void EventBus::Dispatch(Args&&... args)
{
    auto subscribers = retrieve_subscribers(EventHash<EventType>());
    if (subscribers.first == nullptr)
    {
        LOG_POLOS_WARN("[EventBus::Dispatch] No subscribers found for type {}" , EventType{}.Name());
        return;
    }

    auto first = subscribers.first;
    auto last = subscribers.first + subscribers.second;

    std::vector<std::function<void(EventType&)>> subscribers_callbacks(first, last);

    for (auto const& subscriber : subscribers_callbacks)
    {
        EventType event{std::forward<Args>(args)...};
        std::invoke(subscriber, event);
    }
}

template<PolosEvent EventType>
bool Subscribe(std::function<void(EventType&)> t_callback)
{
    return EventBus::Instance().Subscribe(t_callback);
}

template<PolosEvent EventType>
bool Unsubscribe(std::function<void(EventType&)> t_callback)
{
    return EventBus::Instance().Unsubscribe(t_callback);
}

template<PolosEvent EventType, typename... Args>
void Dispatch(Args&&... args)
{
    return EventBus::Instance().Dispatch<EventType>(std::forward<Args>(args)...);
}

} // namespace polos::communication

#endif //CORE_COMMUNICATION_EVENT_BUS_H
