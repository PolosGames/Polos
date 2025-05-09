//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "polos/communication/event_bus/event_bus.hpp"

#include <mutex>
#include <unordered_map>

namespace polos::communication
{

class EventBus::Impl
{
public:
     Impl() = default;
    ~Impl() = default;

    bool Subscribe(std::int64_t const t_event_hash, std::function<void(base_event&)> const& t_callback)
    {
        std::lock_guard lock(m_mutex);

        // Give each subscriber of any event a unique id.
        auto subscriber_id = m_next_id++;

        auto& event_queue = m_callbacks[t_event_hash];
        // Subscriber index is the current tail of the subscriber vector of the specific event.
        std::size_t subscriber_index = event_queue.size();
        event_queue.push_back(std::move(t_callback));

        // The subscriber id will also be the index in our m_indices. In this vector, we keep the actual index of the
        // subscriber (std::function) inside the specific Event's Callback Vector.
        m_indices.push_back(subscriber_index);
        return subscriber_id;
    }

    bool Unsubscribe(std::int64_t const t_event_hash, std::int64_t const t_subscriber_id)
    {
        std::lock_guard lock(m_mutex);

        // Now get the subscriber index back, because subscriber ids are ordered and can be used as indices on the
        // m_indices vector to receive back the actual index.
        // auto subscriber_index = m_indices[static_cast<std::size_t>(t_subscriber_id)];
        // if (auto const it = m_callbacks.find(t_event_type); it != m_callbacks.end())
        // {
        //     auto& vec = it->second;
        //     vec.erase(std::remove_if(vec.begin(), vec.end(),
        //                              [callback_id](const auto& pair) { return pair.first == callback_id; }),
        //               vec.end());
        //     return true;
        // }
        return false;
    }

    [[nodiscard]] std::pair<std::function<void(base_event&)> const*, std::size_t> GetSubscribers(
        std::int64_t const t_event_hash) const
    {
        if (auto it = m_callbacks.find(t_event_hash); it != m_callbacks.end())
        {
            std::function<void(base_event&)> const* data = it->second.data();
            std::size_t size = it->second.size();
            return std::make_pair<>(data, size);
        }

        return {nullptr, 0};
    }
private:
    // To keep indices of the subscribers inside their own Event type_index's.
    std::vector<std::size_t> m_indices;

    // TODO: Make a freelist so we can unsubscribe
    using CallbackMap = std::unordered_map<std::int64_t, std::vector<BaseEventDelegate>>;

    std::int64_t m_next_id{0};
    CallbackMap  m_callbacks;
    std::mutex   m_mutex;
};

EventBus::EventBus()
{
    m_impl = new Impl();
}

EventBus& EventBus::Instance()
{
    static EventBus instance;
    return instance;
}
EventBus::~EventBus()
{
    delete m_impl;
}

std::int64_t EventBus::subscribe_internal(std::int64_t const t_type_hash, std::function<void(base_event&)> const& t_callback) const
{
    return m_impl->Subscribe(t_type_hash, t_callback);
}

bool EventBus::unsubscribe_internal(std::int64_t const t_type_hash, std::int64_t const t_sub_id) const
{
    return m_impl->Unsubscribe(t_type_hash, t_sub_id);
}

std::pair<std::function<void(base_event&)> const*, std::size_t>
EventBus::retrieve_subscribers(std::int64_t const t_type_hash) const
{
    return m_impl->GetSubscribers(t_type_hash);
}

} // namespace polos::communication
