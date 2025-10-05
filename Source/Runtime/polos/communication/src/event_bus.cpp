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

}// namespace polos::communication
