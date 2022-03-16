#include "debug/plassert.h"

#include "pool_allocator.h"

namespace polos::memory
{
	PoolAllocator::PoolAllocator(size_t chunk_size, size_t chunk_amount)
	{
		Initialize(chunk_size, chunk_amount);
	}

	PoolAllocator::~PoolAllocator()
	{
		std::free(m_Buffer);
		m_Buffer       = nullptr;
		m_FreeListHead = nullptr;
	}

	void PoolAllocator::Initialize(size_t chunk_size, size_t chunk_amount)
	{
		PROFILE_FUNC();
		m_BufferSize	= chunk_size * chunk_amount;
		m_ChunkSize		= chunk_size;

		ASSERT(m_ChunkSize > sizeof(free_node) && m_BufferSize > sizeof(free_node));

		m_Buffer        = static_cast<byte*>(std::malloc(m_BufferSize));

		// FreeListHead gets created in Clear function
		Clear();
	}

	void* PoolAllocator::GetNextFree()
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

	void PoolAllocator::Free(void* ptr)
	{
		PROFILE_FUNC();
		ASSERT(!(m_Buffer <= ptr && ptr < m_Buffer + m_BufferSize));

		free_node* node = new (ptr) free_node{ m_FreeListHead };
		m_FreeListHead  = node;
	}

	void PoolAllocator::Clear()
	{
		PROFILE_FUNC();
		memset(m_Buffer, 0, m_BufferSize);

		m_FreeListHead = new (&m_Buffer[0]) free_node{ nullptr };
		auto* itr = m_FreeListHead;

		for (size_t i = m_ChunkSize; i < m_BufferSize; i += m_ChunkSize)
		{
			auto* node = new (&m_Buffer[i]) free_node{ nullptr };
			itr->next  = node;
			itr        = node;
		}
	}
    
    byte* PoolAllocator::Data()
    {
        return m_Buffer;
    }
} // namespace polos::memory