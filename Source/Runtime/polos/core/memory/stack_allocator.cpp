
#include "stack_allocator.h"

namespace polos
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
        
        if (internalBuffer.buffer == nullptr || internalBuffer.bufferSize == 0 || m_Offset == 0)
        {
            LOG_ENGINE_ERROR("[StackAllocator::Pop] Couldn't pop, because the buffer was empty!");
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
            LOG_ENGINE_ERROR("[StackAllocator::Align] Stack Allocator is out of memory! Returning null.");
            return nullptr;
        }

        header              = reinterpret_cast<stack_header*>(m_Bottom + m_Offset);
        header->prevOffset  = m_PrevOffset;
        m_PrevOffset        = m_Offset;
        
        void* ptr = &internalBuffer.buffer[m_Offset + header_size];

        uintptr new_top     = m_Bottom + end_of_new_element;
        uint64 misalignment = new_top & (memory::k_MemoryAlignment - 1);
        uint64 padding      = memory::k_MemoryAlignment - misalignment;
        padding             = padding & (memory::k_MemoryAlignment - 1);

        m_Offset = end_of_new_element + padding;

        return ptr;
    }
    
    byte* StackAllocator::Data()
    {
        return internalBuffer.buffer;
    }
} // namespace polos