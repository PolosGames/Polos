///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_MEMORY_INCLUDE_POLOS_MEMORY_DEBUG_MEMORY_RESOURCE_HPP_
#define POLOS_MEMORY_INCLUDE_POLOS_MEMORY_DEBUG_MEMORY_RESOURCE_HPP_

#include "polos/memory/module_macros.hpp"

#include <cstddef>
#include <cstdint>
#include <memory_resource>
#include <string>

namespace polos::memory
{

class MEMORY_EXPORT DebugMemoryResource : public std::pmr::memory_resource
{
public:
    DebugMemoryResource();
    explicit DebugMemoryResource(std::string                t_name,
                                 std::pmr::memory_resource* t_upstream = std::pmr::new_delete_resource());
    DebugMemoryResource(DebugMemoryResource&&)      = delete;
    DebugMemoryResource(DebugMemoryResource const&) = delete;

    ~DebugMemoryResource();

    DebugMemoryResource& operator=(DebugMemoryResource const&) = delete;
    DebugMemoryResource& operator=(DebugMemoryResource&&)      = delete;

    auto GetMemoryResource() noexcept -> std::pmr::memory_resource*;
    auto GetTotalAllocationInBytes() const -> std::int64_t;
    auto GetUsedMemoryInBytes() const -> std::int64_t;

    void* do_allocate(std::size_t bytes, std::size_t alignment) override;

    void do_deallocate(void* p, std::size_t bytes, std::size_t alignment) override;

    bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override;
private:
    std::int64_t               m_total_allocation{0};
    std::int64_t               m_used_memory_in_bytes{0};
    std::string                m_name;
    std::pmr::memory_resource* m_upstream{nullptr};
};

}// namespace polos::memory

#endif// POLOS_MEMORY_INCLUDE_POLOS_MEMORY_DEBUG_MEMORY_RESOURCE_HPP_
