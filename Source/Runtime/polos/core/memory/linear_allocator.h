#pragma once

#include "polos/core/memory/mem_utils.h"
#include "polos/utils/macro_util.h"
#include "polos/utils/concepts.h"

namespace polos::memory
{
    class LinearAllocator
    {
    public:
        LinearAllocator();
        explicit LinearAllocator(size_t p_Size);
        ~LinearAllocator();
        
        LinearAllocator(LinearAllocator&& p_Other) noexcept;
        LinearAllocator& operator=(LinearAllocator&& p_Rhs) noexcept;
        PL_NO_COPY(LinearAllocator)

        void Initialize(size_t p_Size);
        PL_NODISCARD void* Allocate(size_t p_Size);
        PL_NODISCARD void* Align(size_t p_Size, size_t p_Offset) const;

        template<DefaultConstructible T, typename... Args>
        requires std::is_constructible_v<T, Args...>
        PL_NODISCARD T* New(Args&&... p_Args);

        template<DefaultConstructible T>
        PL_NODISCARD T* NewArr(size_t p_Count);

        template<typename T>
        void Delete(T* p_Ptr);

        template<typename T>
        void DeleteArr(T* p_Ptr, size_t p_Size);
        
        PL_NODISCARD byte*  Data() const;
        PL_NODISCARD size_t Capacity() const;

        void Resize(uint64 p_Size);
        void Clear();
    public:
        internal_buffer internalBuffer;
    private:
        uintptr    m_Bottom;
        size_t     m_Offset;
        std::mutex m_BufferMutex;
    };

    template<DefaultConstructible T, typename... Args>
    requires std::is_constructible_v<T, Args...>
    T* LinearAllocator::New(Args&&... p_Args)
    {
        PROFILE_FUNC();
        return new (Allocate(sizeof(T))) T(std::forward<Args>(p_Args)...);
    }

    template<DefaultConstructible T>
    inline T* LinearAllocator::NewArr(size_t p_Count)
    {
        PROFILE_FUNC();
        return new (Allocate(sizeof(T) * p_Count)) T[p_Count];
    }

    template<typename T>
    void LinearAllocator::Delete(T* p_Ptr)
    {
        p_Ptr->~T();
    }

    template<typename T>
    void LinearAllocator::DeleteArr(T* p_Ptr, size_t p_Size)
    {
        for (size_t i = 0; i < p_Size; ++i)
        {
            p_Ptr[i].~T();
        }
    }
} // namespace polos
