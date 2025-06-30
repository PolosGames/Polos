///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/memory/debug_memory_resource.hpp"
#include "polos/logging/log_macros.hpp"

namespace polos::memory
{

DebugMemoryResource::DebugMemoryResource()
    : DebugMemoryResource{"Default"}
{}

DebugMemoryResource::DebugMemoryResource(std::string t_name, std::pmr::memory_resource* t_upstream)
    : m_name{std::move(t_name)},
      m_upstream{t_upstream}
{}

DebugMemoryResource::~DebugMemoryResource() = default;

auto DebugMemoryResource::GetMemoryResource() noexcept -> std::pmr::memory_resource*
{
    return this;
}

auto DebugMemoryResource::GetTotalAllocationInBytes() const -> std::int64_t
{
    return m_total_allocation;
}

auto DebugMemoryResource::GetUsedMemoryInBytes() const -> std::int64_t
{
    return m_used_memory_in_bytes;
}

void* DebugMemoryResource::do_allocate(std::size_t bytes, std::size_t alignment)
{
    LogDebug("[{} (alloc)] Size: {}, Alignment: {}", m_name, bytes, alignment);
    m_total_allocation += bytes;
    m_used_memory_in_bytes += bytes;
    return m_upstream->allocate(bytes, alignment);
}

void DebugMemoryResource::do_deallocate(void* p, std::size_t bytes, std::size_t alignment)
{
    LogDebug("[{} (dealloc)] Size: {}, Alignment: {}", m_name, bytes, alignment);
    m_used_memory_in_bytes -= bytes;
    m_upstream->deallocate(p, bytes, alignment);
}

bool DebugMemoryResource::do_is_equal(const std::pmr::memory_resource& other) const noexcept
{
    return this == &other;
}

}// namespace polos::memory
