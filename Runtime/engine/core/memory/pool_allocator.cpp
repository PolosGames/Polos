#include "plpch.h"
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

		ASSERT(chunk_size > sizeof(free_node));

		m_Buffer             = static_cast<byte*>(std::malloc(m_BufferSize));
		m_FreeListHead       = reinterpret_cast<free_node*>(&m_Buffer[0]);
		m_FreeListHead->next = nullptr;

		Clear();
	}

	void* PoolAllocator::Get()
	{
		PROFILE_FUNC();
		free_node* node = m_FreeListHead;

		if (!node)
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

		auto* node     = static_cast<free_node*>(ptr);
		node->next     = m_FreeListHead;
		m_FreeListHead = node;
	}

	void PoolAllocator::Clear() const
	{
		PROFILE_FUNC();
		auto* itr = reinterpret_cast<free_node*>(&m_Buffer[0]);

		for (size_t i = m_ChunkSize; i < m_BufferSize; i += m_ChunkSize)
		{
			auto node  = reinterpret_cast<free_node*>(&m_Buffer[i]);
			node->next = nullptr; // make them point to null first.
			itr->next  = node;
			itr        = node;
		}
	}
} // namespace polos::memory