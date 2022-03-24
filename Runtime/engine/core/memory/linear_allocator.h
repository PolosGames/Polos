#pragma once
#ifndef POLOS_CORE_MEMORY_LINEARALLOCATOR_H_
#define POLOS_CORE_MEMORY_LINEARALLOCATOR_H_

#include "mem_utils.h"
#include "utils/macro_util.h"
#include "utils/concepts.h"

namespace polos::memory
{
    class LinearAllocator
    {
    public:
        InternalBuffer iBuffer;
    public:
        LinearAllocator();
        explicit LinearAllocator(size_t size);
        ~LinearAllocator();
        
        LinearAllocator(LinearAllocator&& other) noexcept;
        LinearAllocator& operator=(LinearAllocator&& rhs) noexcept;
        PL_NO_COPY(LinearAllocator);

        void  Initialize(size_t size);
        PL_NODISCARD void* Allocate(size_t size);
        PL_NODISCARD void* Align(size_t size, size_t offset) const;
        
        template<typename T>
        requires IsDefaultConstructable<T>
        PL_NODISCARD T* New();

        template<typename T, typename... Args>
        PL_NODISCARD T* New(Args&&... args);

        template<typename T>
        PL_NODISCARD T* NewArr(size_t count);

        template<typename T>
        void Delete(T* ptr);

        template<typename T>
        void DeleteArr(T* ptr);
        
        PL_NODISCARD byte*  Data() const;
        PL_NODISCARD size_t Capacity() const;

        void Resize(uint64 size);
        void Clear();
    private:
    private:
        uintptr    m_Bottom;
        size_t     m_Offset;
        std::mutex m_BufferMutex;
    };
} // namespace polos

#include "linear_allocator.inl"

#endif /* POLOS_CORE_MEMORY_LINEARALLOCATOR_H_ */