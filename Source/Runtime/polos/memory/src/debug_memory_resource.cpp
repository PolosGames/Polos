//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "polos/memory/debug_memory_resource.hpp"
#include "polos/logging/log_macros.hpp"

namespace polos::memory
{

class DebugMemoryResource::Impl : public std::pmr::memory_resource
{
public:
    explicit Impl(std::string t_name, std::pmr::memory_resource* t_upstream)
        : m_name{std::move(t_name)},
          m_upstream{t_upstream}
    {}

    Impl(Impl&&)      = delete;
    Impl(Impl const&) = delete;

    ~Impl() override = default;

    Impl& operator=(Impl const&) = delete;
    Impl& operator=(Impl&&)      = delete;

    void* do_allocate(std::size_t bytes, std::size_t alignment) override
    {
        LogDebug("[{} (alloc)] Size: {}, Alignment: {}", m_name, bytes, alignment);
        total_allocation += bytes;
        used_memory_in_bytes += bytes;
        return m_upstream->allocate(bytes, alignment);
    }

    void do_deallocate(void* p, std::size_t bytes, std::size_t alignment) override
    {
        LogDebug("[{} (dealloc)] Size: {}, Alignment: {}", m_name, bytes, alignment);
        used_memory_in_bytes -= bytes;
        m_upstream->deallocate(p, bytes, alignment);
    }

    bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override
    {
        return this == &other;
    }

    std::int64_t total_allocation{0};
    std::int64_t used_memory_in_bytes{0};
private:
    std::string                m_name;
    std::pmr::memory_resource* m_upstream{nullptr};
};

DebugMemoryResource::DebugMemoryResource()
    : DebugMemoryResource{"Default"}
{}

DebugMemoryResource::DebugMemoryResource(char const* t_name, std::pmr::memory_resource* t_upstream)
    : m_impl{new Impl{t_name, t_upstream}}
{}

DebugMemoryResource::~DebugMemoryResource()
{
    delete m_impl;
}

auto DebugMemoryResource::GetMemoryResource() const noexcept -> std::pmr::memory_resource*
{
    return m_impl;
}

auto DebugMemoryResource::GetTotalAllocationInBytes() const -> std::int64_t
{
    return m_impl->total_allocation;
}

auto DebugMemoryResource::GetUsedMemoryInBytes() const -> std::int64_t
{
    return m_impl->used_memory_in_bytes;
}

}// namespace polos::memory
