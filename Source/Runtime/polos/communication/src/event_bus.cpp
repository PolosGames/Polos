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
    Impl(polos::memory::DebugAllocator& t_allocator)
        : m_allocator{t_allocator},
          m_callbacks{m_allocator.GetMemoryResource()}
    {}
    ~Impl() = default;

    bool Subscribe(std::int64_t const t_event_hash, std::function<void(base_event&)> const& t_callback)
    {
        std::lock_guard lock(m_mutex);

        // Give each subscriber of any event a unique id.
        auto subscriber_id = m_next_id++;

        m_callbacks.emplace(t_event_hash, std::pmr::vector<BaseEventDelegate>{m_allocator.GetMemoryResource()});
        auto& event_queue = m_callbacks[t_event_hash];
        event_queue.push_back(std::move(t_callback));

        return subscriber_id;
    }

    [[nodiscard]] std::pair<std::function<void(base_event&)> const*, std::size_t>
    GetSubscribers(std::int64_t const t_event_hash) const
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

    polos::memory::DebugAllocator& m_allocator;
    std::int64_t                   m_next_id{0};
    CallbackMap                    m_callbacks;
    std::mutex                     m_mutex;
};

EventBus::EventBus()
    : m_allocator{"EventBus"},
      m_impl{new Impl{m_allocator}}
{}

EventBus& EventBus::Instance()
{
    static EventBus instance;
    return instance;
}

EventBus::~EventBus()
{
    delete m_impl;
}

std::int64_t EventBus::subscribe_internal(std::int64_t const                      t_type_hash,
                                          std::function<void(base_event&)> const& t_callback) const
{
    return m_impl->Subscribe(t_type_hash, t_callback);
}

std::pair<std::function<void(base_event&)> const*, std::size_t>
EventBus::retrieve_subscribers(std::int64_t const t_type_hash) const
{
    return m_impl->GetSubscribers(t_type_hash);
}

}// namespace polos::communication
