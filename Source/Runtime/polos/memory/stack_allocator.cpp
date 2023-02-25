#include "polos/polos_pch.h"

#include "stack_allocator.h"

namespace polos::memory
{
    StackAllocator::StackAllocator(uint64 p_Size)
        : internalBuffer({static_cast<byte*>(std::malloc(p_Size)), p_Size})
        , m_Offset(0)
        , m_PrevOffset(0)
        , m_Bottom(reinterpret_cast<uintptr>(internalBuffer.buffer))
    {
    }
    
    StackAllocator::~StackAllocator()
    {
        PROFILE_FUNC();
        if (internalBuffer.buffer != nullptr)
        {
            std::free(internalBuffer.buffer);
        }
        internalBuffer.buffer = nullptr;
    }
    
    StackAllocator::StackAllocator(StackAllocator&& p_Other) noexcept
        : internalBuffer{std::exchange(p_Other.internalBuffer, {})}
        , m_Offset{std::exchange(p_Other.m_Offset, 0)}
        , m_PrevOffset{std::exchange(p_Other.m_PrevOffset, 0)}
        , m_Bottom{}
    {
    }
    
    StackAllocator& StackAllocator::operator=(StackAllocator&& p_Rhs) noexcept
    {
        internalBuffer = std::exchange(p_Rhs.internalBuffer, {});
        m_Offset       = std::exchange(p_Rhs.m_Offset, 0);
        m_PrevOffset   = std::exchange(p_Rhs.m_PrevOffset, 0);
        m_Bottom       = std::exchange(p_Rhs.m_Bottom, 0);
        return *this;
    }

    void StackAllocator::Pop()
    {
        PROFILE_FUNC();
        if (!internalBuffer.buffer || internalBuffer.bufferSize == 0 || m_Offset == 0)
        {
            ASSERTSTR(0, "[StackAllocator::Pop] Cannot pop because the buffer is empty!");
            return;
        }

        uint64 prevOffset = std::launder(reinterpret_cast<stack_header*>(&internalBuffer.buffer[m_PrevOffset]))->prevOffset;

        m_Offset     = m_PrevOffset;
        m_PrevOffset = prevOffset;
    }

    void StackAllocator::Clear()
    {
        PROFILE_FUNC();
        m_PrevOffset = 0;
        m_Offset     = m_Bottom;
        std::memset(internalBuffer.buffer, 0, internalBuffer.bufferSize);
    }

    void* StackAllocator::align(uint64 p_Size)
    {
        PROFILE_FUNC();
        stack_header* header;

        uint32 header_size        = sizeof(stack_header);
        uint64 end_of_new_element = m_Offset + header_size + p_Size;// the new unaligned offset

        if (end_of_new_element > internalBuffer.bufferSize)
        {
            ASSERTSTR(0, "Stack Allocator is out of memory! Returning null. (StackAllocator::Align)");
            return nullptr;
        }

        header              = reinterpret_cast<stack_header*>(m_Bottom + m_Offset);
        header->prevOffset  = m_PrevOffset;
        m_PrevOffset        = m_Offset;
        
        void* ptr = &internalBuffer.buffer[m_Offset + header_size];

        uintptr new_top     = m_Bottom + end_of_new_element;
        uint64 misalignment = new_top & (MemUtils::k_MemoryAlignment - 1);
        uint64 padding      = MemUtils::k_MemoryAlignment - misalignment;
        padding             = padding & (MemUtils::k_MemoryAlignment - 1);

        m_Offset = end_of_new_element + padding;

        return ptr;
    }
    
    byte* StackAllocator::Data()
    {
        return internalBuffer.buffer;
    }
} // namespace polos