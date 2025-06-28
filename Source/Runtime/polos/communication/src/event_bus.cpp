//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "polos/communication/event_bus.hpp"

#include <mutex>
#include <unordered_map>
#include <vector>

namespace polos::communication
{

class EventBus::Impl
{
public:
    Impl(polos::memory::DebugMemoryResource& t_allocator)
        : m_allocator{t_allocator},
          m_callbacks{m_allocator.GetMemoryResource()}
    {}
    ~Impl() = default;

    auto Subscribe(std::int64_t const t_event_hash, std::function<void(base_event&)> const& t_callback) -> bool
    {
        std::lock_guard lock(m_mutex);

        // Give each subscriber of any event a unique id.
        auto subscriber_id = m_next_id++;

        m_callbacks.emplace(t_event_hash, std::pmr::vector<BaseEventDelegate>{m_allocator.GetMemoryResource()});
        auto& event_queue = m_callbacks[t_event_hash];
        event_queue.push_back(t_callback);

        return subscriber_id;
    }

    auto GetSubscribers(std::int64_t const t_event_hash) const
        -> std::pair<std::function<void(base_event&)> const*, std::size_t>
    {
        if (auto it = m_callbacks.find(t_event_hash); it != m_callbacks.end())
        {
            std::function<void(base_event&)> const* data = it->second.data();
            std::size_t                             size = it->second.size();
            return std::make_pair<>(data, size);
        }

        return {nullptr, 0};
    }
private:
    // TODO: Make a freelist so we can unsubscribe
    using CallbackMap = std::pmr::unordered_map<std::int64_t, std::pmr::vector<BaseEventDelegate>>;

    polos::memory::DebugMemoryResource& m_allocator;
    std::int64_t                        m_next_id{0};
    CallbackMap                         m_callbacks;
    std::mutex                          m_mutex;
};

EventBus::EventBus()
    : m_allocator{"EventBus"},
      m_impl{new Impl{m_allocator}}
{}

auto EventBus::Instance() -> EventBus&
{
    static EventBus instance;
    return instance;
}

EventBus::~EventBus()
{
    delete m_impl;
}

auto EventBus::subscribe_internal(std::int64_t const                      t_type_hash,
                                  std::function<void(base_event&)> const& t_callback) const -> std::int64_t
{
    return m_impl->Subscribe(t_type_hash, t_callback);
}

auto EventBus::retrieve_subscribers(std::int64_t const t_type_hash) const
    -> std::pair<std::function<void(base_event&)> const*, std::size_t>
{
    return m_impl->GetSubscribers(t_type_hash);
}

}// namespace polos::communication
