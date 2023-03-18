#pragma once

#include "polos/core/memory/mem_utils.h"
#include "polos/utils/macro_util.h"
#include "polos/utils/concepts.h"

namespace polos
{
    class StackAllocator
    {
    private:
        struct stack_header
        {
            uint64 prevOffset; /// Stores previous element's header's start
        };
    public:
        explicit StackAllocator(uint64 p_Size);
        ~StackAllocator();

        StackAllocator(StackAllocator const&) = delete;
        StackAllocator& operator=(StackAllocator const&) = delete;
        StackAllocator(StackAllocator&& p_Other) noexcept;
        StackAllocator& operator=(StackAllocator&& p_Rhs) noexcept;

        template<typename T, typename... Args>
            requires(std::is_constructible_v<T, Args...>)
        T* Push(Args&&... p_Args);

        template<DefaultConstructible T>
        T* PushArr(uint64 p_Count);
        
        byte* Data();

        void Pop();
        void Clear();
    private:
        PL_NODISCARD void* align(uint64 p_Size);
    public:
        memory::internal_buffer internalBuffer;
    private:
        uint64  m_Offset;
        uint64  m_PrevOffset;
        uintptr m_Bottom;
    };

    template<typename T, typename... Args>
        requires(std::is_constructible_v<T, Args...>)
    T* StackAllocator::Push(Args&&... p_Args)
    {
        PROFILE_FUNC();
        return new (align(sizeof(T))) T(std::forward<Args>(p_Args)...);
    }

    template<DefaultConstructible T>
    T* StackAllocator::PushArr(uint64 p_Count)
    {
        PROFILE_FUNC();
        return new (align(sizeof(T) * p_Count)) T[p_Count];
    }
} // namespace polos
