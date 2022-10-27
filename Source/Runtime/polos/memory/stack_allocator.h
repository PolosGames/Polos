#pragma once
#ifndef POLOS_CORE_MEMORY_STACKALLOCATOR_H_
#define POLOS_CORE_MEMORY_STACKALLOCATOR_H_

#include "polos/memory/mem_utils.h"
#include "polos/utils/macro_util.h"

namespace polos::memory
{
    class StackAllocator
    {
        struct stack_header
        {
            uint64 prevOffset; /// Stores previous element's header's start
        };
    public:
        internal_buffer iBuffer;
    public:
        explicit StackAllocator(uint64 size);
        ~StackAllocator();
        
        StackAllocator(StackAllocator&& other) noexcept;
        StackAllocator& operator=(StackAllocator&& rhs) noexcept;
        
        PL_NO_COPY(StackAllocator)

        template<typename T, typename... Args>
        T* Push(Args&&... args);

        template<typename T>
        T* PushArr(uint64 count);
        
        byte* Data();

        void Pop();
        void Clear();
    private:
        PL_NODISCARD void* align(uint64 size);
    private:
        uint64  m_Offset;
        uint64  m_PrevOffset;
        uintptr m_Bottom;
    };
} // namespace polos

#include "stack_allocator.inl"

#endif /* POLOS_CORE_MEMORY_STACKALLOCATOR_H_ */