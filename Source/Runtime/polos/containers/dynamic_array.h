#pragma once
#ifndef POLOS_CORE_CONTAINERS_DARRAY_H_
#define POLOS_CORE_CONTAINERS_DARRAY_H_

#include "polos/memory/pool_allocator.h"
#include "polos/utils/concepts.h"

namespace polos::containers
{
    template<class T>
    class DynamicArray
    {
        using ValueType      = T;
        using Pointer        = T*;
        using ConstPointer   = T const*;
        using Reference      = T&;
        using ConstReference = T const&;
    public:
        DynamicArray()
            : m_ElementCount(0),
              m_ValueSize(sizeof(ValueType)),
              m_ResizeAmountBytes(m_ValueSize * kResizeAmount)
        {
            // initialize with 16 empty elements
            m_Allocator.Initialize( kResizeAmount );
        }
        
        ConstReference PushBack(ValueType const& element)
        requires CopyConstructible<ValueType>
        {
            if(m_Allocator.Capacity() == m_ElementCount)
            {
                ConstReference p = resize_push(element);
                m_ElementCount++;
                return p;
            }
            m_ElementCount++;
            new (m_Allocator.get_next_free()) ValueType(element);
        }
    
        template<typename ...Args>
        Reference EmplaceBack(Args&&... args)
        {
            new (m_Allocator.get_next_free()) ValueType(std::forward<Args>(args)...);
        }
        
        PL_NODISCARD size_t Size() const
        {
            return m_ElementCount;
        }
    
        PL_NODISCARD Reference operator[](size_t index)
        {
            return *(m_Allocator.Data() + index);
        }
        
        DynamicArray& operator=(DynamicArray<T> const& rhs) noexcept
        {
            if(&rhs == this) return *this;
            m_Allocator.Initialize(rhs.m_Allocator.Capacity());
            m_ElementCount = rhs.m_ElementCount;
            
            std::memcpy(m_Allocator.Data(), rhs.m_Allocator.Data(), m_Allocator.Capacity());
        }
        
        DynamicArray& operator=(DynamicArray<T>&& rhs) noexcept
        {
            if(&rhs == this) return *this;
            m_Allocator = std::move(rhs.m_Allocator);
        }
    private:
        PL_NODISCARD ConstReference resize_push(ConstReference element)
        {
            m_Allocator.Resize(m_Allocator.iBuffer.bufferSize + m_ResizeAmountBytes);
            ValueType* ptr = new (m_Allocator.Allocate(m_ValueSize)) ValueType(element);
            return *ptr;
        }
    private:
        inline static constexpr int32 kResizeAmount = 16;
        memory::PoolAllocator<ValueType> m_Allocator;
        size_t m_ElementCount;
        size_t m_ValueSize;
        size_t m_ResizeAmountBytes;
    };
}

#endif /* POLOS_CORE_CONTAINERS_DARRAY_H_ */