#pragma once
#ifndef POLOS_CORE_MEMORY_POOLALLOCATOR_H_
#define POLOS_CORE_MEMORY_POOLALLOCATOR_H_

#include "polos/memory/mem_utils.h"
#include "polos/utils/macro_util.h"
#include "polos/utils/concepts.h"

namespace polos::memory
{
    template<typename T>
    requires DefaultConstructible<T> &&
                 CopyConstructible<T> &&
                 CopyAssignable<T> &&
                 MoveConstructible<T> &&
                 MoveAssignable<T>
    class PoolAllocator
    {
        struct free_node
        {
            free_node* next;
        };
    public:
        internal_buffer iBuffer;
    public:
        PL_NO_COPY(PoolAllocator);
        
        PoolAllocator()
            : iBuffer({nullptr, 0}),
              m_FreeListHead(nullptr),
              m_ChunkSize{},
              m_ChunkAmount{}
        {
        }
        
        explicit PoolAllocator(size_t amount)
            : iBuffer{static_cast<byte*>(std::malloc(sizeof(T) * amount)), sizeof(T) * amount},
              m_ChunkSize{sizeof(T)},
              m_ChunkAmount{amount}
        {
            ASSERT(m_ChunkSize > sizeof(free_node) && iBuffer.bufferSize > sizeof(free_node));
            // FreeListHead gets created in Clear function
            Clear();
        }
        
        ~PoolAllocator()
        {
            if(iBuffer.buffer != nullptr)
                std::free(iBuffer.buffer);
            iBuffer.buffer       = nullptr;
            m_FreeListHead = nullptr;
        }
        
        PoolAllocator(PoolAllocator&& other) noexcept
            : iBuffer       (std::exchange(other.iBuffer, {nullptr, 0})),
              m_FreeListHead(std::exchange(other.m_FreeListHead, nullptr)),
              m_ChunkSize   (std::exchange(other.m_ChunkSize, 0)),
              m_ChunkAmount (std::exchange(other.m_ChunkAmount, 0))
        {
        }
        
        PoolAllocator& operator=(PoolAllocator&& rhs) noexcept
        {
            if(&rhs == this) return *this;
            iBuffer        = std::exchange(rhs.iBuffer, {nullptr, 0});
            m_FreeListHead = std::exchange(rhs.m_FreeListHead, nullptr);
            m_ChunkSize    = std::exchange(rhs.m_ChunkSize, 0);
            m_ChunkAmount  = std::exchange(rhs.m_ChunkAmount, 0);
            return *this;
        }

        void Initialize(size_t amount)
        {
            PROFILE_FUNC();
            m_ChunkSize        = sizeof(T);
            m_ChunkAmount      = amount;
            iBuffer.bufferSize = m_ChunkSize * m_ChunkAmount;
            iBuffer.buffer     = static_cast<byte*>(std::malloc(iBuffer.bufferSize));
            
            ASSERT(m_ChunkSize > sizeof(free_node) && iBuffer.bufferSize > sizeof(free_node));
            
            // FreeListHead gets created in Clear function
            Clear();
        }
        
        void Resize(size_t amount)
        {
            if(amount <= m_ChunkAmount)
            {
                // just shrink the buffer size instead of reallocating the buffer.
                iBuffer.bufferSize = amount * m_ChunkSize;
                // Clear with the new buffer size
                Clear();
                return;
            }
            
            byte* old_buffer = iBuffer.buffer;
            
            size_t new_buffer_size = amount * m_ChunkSize;
            iBuffer.buffer         = static_cast<byte*>(std::malloc(new_buffer_size));
    
            // destruct all objects inside
            for(size_t i = 0; i < iBuffer.bufferSize; i += m_ChunkSize)
            {
                reinterpret_cast<T*>(&iBuffer.buffer[i])->~T();
            }
            
            iBuffer.bufferSize  = new_buffer_size;
            m_ChunkAmount       = amount;
            std::free(old_buffer);
            
            Clear();
        }
        
        PL_NODISCARD size_t Capacity() const
        {
            return m_ChunkAmount;
        }
        
        PL_NODISCARD size_t ByteCapacity() const
        {
            return iBuffer.bufferSize;
        }
        
        PL_NODISCARD T* Data() const
        {
            return reinterpret_cast<T*>(iBuffer.buffer);
        }
        
        void Free(void*& ptr)
        {
            PROFILE_FUNC();
            ASSERT(!(iBuffer.buffer <= ptr && ptr < iBuffer.buffer + iBuffer.bufferSize));
    
            static_cast<T*>( ptr )->~T();
            auto* node     = new (ptr) free_node{ m_FreeListHead };
            m_FreeListHead = node;
            ptr = nullptr;
        }
        
        void Clear()
        {
            PROFILE_FUNC();
            // destruct all objects inside
            for(size_t i = 0; i < iBuffer.bufferSize; i += m_ChunkSize)
            {
                reinterpret_cast<T*>(&iBuffer.buffer[i])->~T();
            }
            std::memset(iBuffer.buffer, 0, iBuffer.bufferSize);
    
            m_FreeListHead = new (&iBuffer.buffer[0]) free_node{ nullptr };
            auto* itr      = m_FreeListHead;
    
            for (size_t i = m_ChunkSize; i < iBuffer.bufferSize; i += m_ChunkSize)
            {
                auto* node = new (&iBuffer.buffer[i]) free_node{ nullptr };
                itr->next  = node;
                itr        = node;
            }
        }
        
    private:
        PL_NODISCARD void* get_next_free()
        {
            PROFILE_FUNC();
            free_node* node = m_FreeListHead;
            
            if (node == nullptr)
            {
                ASSERTSTR(0, "PoolAllocator is out of memory.");
                return nullptr;
            }
            
            m_FreeListHead = m_FreeListHead->next;
            
            return node;
        }
    private:
        free_node* m_FreeListHead;
        size_t     m_ChunkSize;
        size_t     m_ChunkAmount;
    };
} // namespace polos::memory

#endif /* POLOS_CORE_MEMORY_POOLALLOCATOR_H_ */