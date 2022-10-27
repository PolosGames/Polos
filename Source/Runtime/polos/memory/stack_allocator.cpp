#include "polos/polos_pch.h"

#include "stack_allocator.h"

namespace polos::memory
{
    StackAllocator::StackAllocator(uint64 size)
        : iBuffer({static_cast<byte*>(std::malloc(size)), size}),
          m_Offset(0),
          m_PrevOffset(0),
          m_Bottom(reinterpret_cast<uintptr>(iBuffer.buffer))
    {
    }
    
    StackAllocator::~StackAllocator()
    {
        PROFILE_FUNC();
        if(iBuffer.buffer != nullptr)
            std::free(iBuffer.buffer);
        iBuffer.buffer = nullptr;
    }
    
    StackAllocator::StackAllocator(StackAllocator&& other) noexcept
        : iBuffer      (std::exchange(other.iBuffer, {nullptr, 0})),
          m_Offset     (std::exchange(other.m_Offset, 0)),
          m_PrevOffset (std::exchange(other.m_PrevOffset, 0)),
          m_Bottom     (std::exchange(other.m_Bottom, 0))
    {
    }
    
    StackAllocator& StackAllocator::operator=(StackAllocator&& rhs) noexcept
    {
        iBuffer      = std::exchange(rhs.iBuffer, {nullptr, 0});
        m_Offset     = std::exchange(rhs.m_Offset, 0);
        m_PrevOffset = std::exchange(rhs.m_PrevOffset, 0);
        m_Bottom     = std::exchange(rhs.m_Bottom, 0);
        return *this;
    }

    void StackAllocator::Pop()
    {
        PROFILE_FUNC();
        if (!iBuffer.buffer || iBuffer.bufferSize == 0 || m_Offset == 0)
        {
            ASSERTSTR(0, "Cannot pop because the buffer is empty! (StackAllocator::Pop)");
            return;
        }

        uint64 prevOffset = reinterpret_cast<stack_header*>(&iBuffer.buffer[m_PrevOffset])->prevOffset;

        m_Offset     = m_PrevOffset;
        m_PrevOffset = prevOffset;
    }

    void StackAllocator::Clear()
    {
        PROFILE_FUNC();
        m_PrevOffset = 0;
        m_Offset     = m_Bottom;
        std::memset(iBuffer.buffer, 0, iBuffer.bufferSize);
    }

    void* StackAllocator::align(uint64 size)
    {
        PROFILE_FUNC();
        stack_header* header;

        uint32 header_size        = sizeof(stack_header);
        uint64 end_of_new_element = m_Offset + header_size + size; // the new unaligned offset

        if (end_of_new_element > iBuffer.bufferSize)
        {
            ASSERTSTR(0, "Stack Allocator is out of memory! Returning null. (StackAllocator::Align)");
            return nullptr;
        }

        header              = reinterpret_cast<stack_header*>(m_Bottom + m_Offset);
        header->prevOffset = m_PrevOffset;
        m_PrevOffset        = m_Offset;
        
        void* ptr = &iBuffer.buffer[m_Offset + header_size];

        uintptr new_top     = m_Bottom + end_of_new_element;
        uint64 misalignment = new_top & (MemUtils::kMemoryAlignment - 1);
        uint64 padding      = MemUtils::kMemoryAlignment - misalignment;
        padding             = padding & (MemUtils::kMemoryAlignment - 1);

        m_Offset = end_of_new_element + padding;

        return ptr;
    }
    
    byte* StackAllocator::Data()
    {
        return iBuffer.buffer;
    }
} // namespace polos