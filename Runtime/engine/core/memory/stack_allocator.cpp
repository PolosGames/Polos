#include <cstring>

#include "mem_utils.h"
#include "utils/alias.h"
#include "debug/plassert.h"

#include "stack_allocator.h"

namespace polos::memory
{
	StackAllocator::StackAllocator(uint64 size)
		: m_BufferSize(size)
	{
		PROFILE_FUNC();
		m_Buffer     = static_cast<byte*>(std::malloc(size));
		m_Offset     = 0;
		m_PrevOffset = 0;
		m_Bottom     = reinterpret_cast<uintptr>(m_Buffer);
	}
	
	StackAllocator::~StackAllocator()
	{
		PROFILE_FUNC();
		std::free(m_Buffer);
		m_Buffer = nullptr;
	}

	void StackAllocator::Pop()
	{
		PROFILE_FUNC();
		if (!m_Buffer || m_BufferSize == 0 || m_Offset == 0)
		{
			ASSERTSTR(0, "Cannot pop because the buffer is empty! (StackAllocator::Pop)");
			return;
		}

		uint64 prev_offset = reinterpret_cast<stack_header*>(&m_Buffer[m_PrevOffset])->prev_offset;

		m_Offset     = m_PrevOffset;
		m_PrevOffset = prev_offset;
	}

	void StackAllocator::Clear()
	{
		PROFILE_FUNC();
		m_PrevOffset = 0;
		m_Offset     = m_Bottom;
		std::memset(m_Buffer, 0, m_BufferSize);
	}

	void* StackAllocator::align(uint64 size)
	{
		PROFILE_FUNC();
		stack_header* header;

		uint32 header_size        = sizeof(stack_header);
		uint64 end_of_new_element = m_Offset + header_size + size; // the new unaligned offset

		if (end_of_new_element > m_BufferSize)
		{
			ASSERTSTR(0, "Stack Allocator is out of memory! Returning null. (StackAllocator::align)");
			return nullptr;
		}

		header              = reinterpret_cast<stack_header*>(m_Bottom + m_Offset);
		header->prev_offset = m_PrevOffset;
		m_PrevOffset        = m_Offset;
		
		void* ptr = &m_Buffer[m_Offset + header_size];

		uintptr new_top     = m_Bottom + end_of_new_element;
		uint64 misalignment = new_top & (MemUtils::kMemoryAlignment - 1);
		uint64 padding      = MemUtils::kMemoryAlignment - misalignment;
		padding             = padding & (MemUtils::kMemoryAlignment - 1);

		m_Offset = end_of_new_element + padding;

		return ptr;
	}
    
    byte* StackAllocator::Data()
    {
        return m_Buffer;
    }
} // namespace polos