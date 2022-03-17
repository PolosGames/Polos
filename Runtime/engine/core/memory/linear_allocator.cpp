#include <cstdlib>
#include <cstring>

#include "mem_utils.h"
#include "debug/plassert.h"

#include "linear_allocator.h"

namespace polos::memory
{
	LinearAllocator::LinearAllocator(uint64 size)
		: m_BufferSize(size)
	{
		PROFILE_FUNC();
		m_Buffer = static_cast<byte*>(std::malloc(size));
		m_Offset = 0;
		m_Bottom = reinterpret_cast<uintptr>(m_Buffer);
	}

	LinearAllocator::~LinearAllocator()
	{
		PROFILE_FUNC();
		if(m_Buffer != nullptr)
		    std::free(m_Buffer);
		m_Buffer = nullptr;
	}

	void* LinearAllocator::Allocate(uint64 size)
	{
		return align(size);
	}

	void LinearAllocator::Resize(uint64 size)
	{
		if (size <= m_BufferSize)
		{
			m_Offset     = size;
			m_BufferSize = size;
		}
		else
		{
			void* old_mem = m_Buffer;
			m_Buffer      = static_cast<byte*>(std::malloc(size));

			if (!m_Buffer)
			{
				ASSERTSTR(0, "Buffer is null! (LinearAllocator::Resize)");
				return;
			}

			// Need this bc memcpy is not safe, and this looks fancier :)
			// Could've used memmove which is safer.
			std::lock_guard<std::mutex> lock(m_BufferMutex);
			std::memcpy(m_Buffer, old_mem, m_BufferSize);
			
			std::free(old_mem);
			old_mem = nullptr; // Probably unnecessary lol.
		}
	}

	void LinearAllocator::Clear()
	{
		m_Offset = 0;
	}

	void* LinearAllocator::align(uint64 size)
	{
		uint64 new_offset = m_Offset + size;

		if (new_offset > m_BufferSize)
		{
			ASSERTSTR(0, "LinearAllocator is full.");
			return nullptr;
		}

		void* ptr = &m_Buffer[m_Offset];

		uintptr curr_ptr    = m_Bottom + m_Offset;
		uint64 misalignment = curr_ptr & (MemUtils::kMemoryAlignment - 1);
		uint64 padding      = MemUtils::kMemoryAlignment - misalignment;
		padding             = padding & (MemUtils::kMemoryAlignment - 1); // if misalignment == 0, padding becomes 16

		m_Offset = new_offset + padding;
		return ptr;
	}
    
    byte* LinearAllocator::Data()
    {
        return m_Buffer;
    }
}