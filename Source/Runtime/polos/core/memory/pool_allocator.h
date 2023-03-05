#pragma once

#include "polos/memory/mem_utils.h"
#include "polos/utils/macro_util.h"
#include "polos/utils/concepts.h"

namespace polos::memory
{
    template<typename T>
    class PoolAllocator
    {
    private:
        struct free_node
        {
            free_node* next;
        };
    public:
        PoolAllocator();
        ~PoolAllocator();

        explicit PoolAllocator(size_t p_Amount);
        PoolAllocator(PoolAllocator&& other) noexcept = default;
        PoolAllocator& operator=(PoolAllocator&& rhs) noexcept = default;

        PL_NO_COPY(PoolAllocator);

        void Initialize(size_t amount);
        void Resize(size_t amount);
        
        PL_NODISCARD size_t Capacity() const
        PL_NODISCARD size_t ByteCapacity() const;
        
        PL_NODISCARD T* Data() const;
        void Free(void*& ptr);
        void Clear();
        
    private:
        PL_NODISCARD void* get_next_free();
    public:
        internal_buffer internalBuffer;
    private:
        free_node*  m_FreeListHead;
        std::size_t m_ChunkSize;
        std::size_t m_ChunkAmount;
    };
    
    template<typename T>
    PoolAllocator<T>::PoolAllocator()
        : internalBuffer{}
        , m_FreeListHead{}
        , m_ChunkSize{}
        , m_ChunkAmount{}
    {
    }

    template<typename T>
    PoolAllocator<T>::~PoolAllocator()
    {
        if (internalBuffer.buffer != nullptr)
        {
            std::free(internalBuffer.buffer);
        }
        internalBuffer.buffer = nullptr;
        m_FreeListHead        = nullptr;
    }

    template<typename T>
    PoolAllocator<T>::PoolAllocator(std::size_t p_Amount)
        : internalBuffer{static_cast<byte*>(std::malloc(sizeof(T) * p_Amount), sizeof(T) * p_Amount}
        , m_ChunkSize{sizeof(T)}
        , m_ChunkAmount{p_Amount}
    {
        ASSERT(m_ChunkSize > sizeof(free_node) && internalBuffer.bufferSize > sizeof(free_node));
        // FreeListHead gets created in Clear function
        Clear();
    }

    template<typename T>
    void PoolAllocator<T>::Initialize(size_t amount)
    {
        PROFILE_FUNC();
        m_ChunkSize               = sizeof(T);
        m_ChunkAmount             = amount;
        internalBuffer.bufferSize = m_ChunkSize * m_ChunkAmount;
        internalBuffer.buffer     = static_cast<byte*>(std::malloc(internalBuffer.bufferSize));

        ASSERT(m_ChunkSize > sizeof(free_node) && internalBuffer.bufferSize > sizeof(free_node));

        // FreeListHead gets created in Clear function
        Clear();
    }
    
    template<typename T>
    void PoolAllocator<T>::Resize(size_t amount)
    {
        if (amount <= m_ChunkAmount)
        {
            // just shrink the buffer size instead of reallocating the buffer.
            internalBuffer.bufferSize = amount * m_ChunkSize;
            // Clear with the new buffer size
            Clear();
            return;
        }

        byte* old_buffer = internalBuffer.buffer;

        size_t new_buffer_size = amount * m_ChunkSize;
        internalBuffer.buffer  = static_cast<byte*>(std::malloc(new_buffer_size));

        // destruct all objects inside
        for (size_t i = 0; i < internalBuffer.bufferSize; i += m_ChunkSize)
        {
            reinterpret_cast<T*>(&internalBuffer.buffer[i])->~T();
        }

        internalBuffer.bufferSize = new_buffer_size;
        m_ChunkAmount             = amount;
        std::free(old_buffer);

        Clear();
    }
    
    template<typename T>
    size_t PoolAllocator<T>::Capacity() const
    {
        return m_ChunkAmount;
    }
    template<typename T>
    size_t PoolAllocator<T>::ByteCapacity() const
    {
        return internalBuffer.bufferSize;
    }
    
    template<typename T>
    T* PoolAllocator<T>::Data() const
    {
        return std::launder(reinterpret_cast<T*>(internalBuffer.buffer));
    }

    template<typename T>
    void PoolAllocator<T>::Free(void*& ptr)
    {
        PROFILE_FUNC();
        ASSERT(!(internalBuffer.buffer <= ptr && ptr < internalBuffer.buffer + internalBuffer.bufferSize));

        static_cast<T*>(ptr)->~T();
        auto* node     = new (ptr) free_node{m_FreeListHead};
        m_FreeListHead = node;
        ptr            = nullptr;
    }
    template<typename T>
    void PoolAllocator<T>::Clear()
    {
        PROFILE_FUNC();
        // destruct all objects inside
        for (size_t i = 0; i < internalBuffer.bufferSize; i += m_ChunkSize)
        {
            reinterpret_cast<T*>(&internalBuffer.buffer[i])->~T();
        }
        std::memset(internalBuffer.buffer, 0, internalBuffer.bufferSize);

        m_FreeListHead = new (&internalBuffer.buffer[0]) free_node{nullptr};
        auto* itr      = m_FreeListHead;

        for (size_t i = m_ChunkSize; i < internalBuffer.bufferSize; i += m_ChunkSize)
        {
            auto* node = new (&internalBuffer.buffer[i]) free_node{nullptr};
            itr->next  = node;
            itr        = node;
        }
    }
    
    template<typename T>
    PL_NODISCARD void* PoolAllocator<T>::get_next_free()
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
}// namespace polos::memory
