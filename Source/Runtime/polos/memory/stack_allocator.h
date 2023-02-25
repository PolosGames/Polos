#pragma once

#include "polos/memory/mem_utils.h"
#include "polos/utils/macro_util.h"
#include "polos/utils/concepts.h"

namespace polos::memory
{
    class StackAllocator
    {
    private:
        struct stack_header
        {
            uint64 prevOffset; /// Stores previous element's header's start
        };
    public:
        explicit StackAllocator(uint64 size);
        ~StackAllocator();
        
        StackAllocator(StackAllocator&& other) noexcept;
        StackAllocator& operator=(StackAllocator&& rhs) noexcept;
        
        PL_NO_COPY(StackAllocator)

        template<typename T, typename... Args>
            requires(std::is_constructible_v<T, Args...>)
        T* Push(Args&&... p_Args);

        template<DefaultConstructible T>
        T* PushArr(uint64 count);
        
        byte* Data();

        void Pop();
        void Clear();
    private:
        PL_NODISCARD void* align(uint64 size);
    public:
        internal_buffer internalBuffer;
    private:
        uint64  m_Offset;
        uint64  m_PrevOffset;
        uintptr m_Bottom;
    };

    template<typename T, typename... Args>
        requires(std::is_constructible_v<T, Args...>)
    T* StackAllocator::Push(Args&&... args)
    {
        PROFILE_FUNC();
        return new (align(sizeof(T))) T(std::forward<Args>(args)...);
    }

    template<DefaultConstructible T>
    T* StackAllocator::PushArr(uint64 count)
    {
        PROFILE_FUNC();
        return new (align(sizeof(T) * count)) T[count];
    }
} // namespace polos
