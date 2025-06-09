//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef POLOS_MEMORY_INCLUDE_POLOS_MEMORY_DEBUG_ALLOCATOR_HPP_
#define POLOS_MEMORY_INCLUDE_POLOS_MEMORY_DEBUG_ALLOCATOR_HPP_

#include "polos/memory/module_macros.hpp"

#include <cstddef>
#include <cstdint>
#include <memory_resource>
#include <string>

namespace polos::memory
{

class MEMORY_EXPORT DebugAllocator
{
public:
    DebugAllocator();
    explicit DebugAllocator(char const*                t_name,
                            std::pmr::memory_resource* t_upstream = std::pmr::new_delete_resource());
    DebugAllocator(DebugAllocator&&)      = delete;
    DebugAllocator(DebugAllocator const&) = delete;

    ~DebugAllocator();

    DebugAllocator& operator=(DebugAllocator const&) = delete;
    DebugAllocator& operator=(DebugAllocator&&)      = delete;

    auto GetMemoryResource() const noexcept -> std::pmr::memory_resource*;
    auto GetTotalAllocationInBytes() const -> std::int64_t;
    auto GetUsedMemoryInBytes() const -> std::int64_t;
private:
    class Impl;
    Impl* m_impl;
};

}// namespace polos::memory

#endif//POLOS_MEMORY_INCLUDE_POLOS_MEMORY_DEBUG_ALLOCATOR_HPP_
