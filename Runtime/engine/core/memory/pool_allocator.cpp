#include "debug/plassert.h"

#include "pool_allocator.h"

namespace polos::memory
{
    PoolAllocator::PoolAllocator()
        : m_Buffer(nullptr), m_BufferSize{}, m_ChunkSize{}, m_ChunkAmount{}
    {
    }

	PoolAllocator::PoolAllocator(size_t chunk_size, size_t chunk_amount)
	{
		Initialize(chunk_size, chunk_amount);
	}

	PoolAllocator::~PoolAllocator()
	{
	    if(m_Buffer != nullptr)
		    std::free(m_Buffer);
		m_Buffer       = nullptr;
		m_FreeListHead = nullptr;
	}
    
    PoolAllocator::PoolAllocator(PoolAllocator&& other) noexcept
    {
        if(other.m_Buffer == this->m_Buffer) return;
        m_Buffer       = other.m_Buffer;
        m_BufferSize   = other.m_BufferSize;
        m_ChunkAmount  = other.m_ChunkAmount;
        m_ChunkSize    = other.m_ChunkSize;
        m_FreeListHead = other.m_FreeListHead;
        
        other.m_Buffer = nullptr;
        other.m_FreeListHead = nullptr;
    }
    
    PoolAllocator& PoolAllocator::operator=(PoolAllocator&& rhs) noexcept
    {
        if(rhs.m_Buffer == this->m_Buffer) return *this;
        m_Buffer       = rhs.m_Buffer;
        m_BufferSize   = rhs.m_BufferSize;
        m_ChunkAmount  = rhs.m_ChunkAmount;
        m_ChunkSize    = rhs.m_ChunkSize;
        m_FreeListHead = rhs.m_FreeListHead;
    
        rhs.m_Buffer = nullptr;
        rhs.m_FreeListHead = nullptr;
        return *this;
    }

	void PoolAllocator::Initialize(size_t chunk_size, size_t chunk_amount)
	{
		PROFILE_FUNC();
		m_BufferSize        = chunk_size * chunk_amount;
		m_ChunkSize         = chunk_size;
		m_ChunkAmount       = chunk_amount;

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
    
    void PoolAllocator::Resize(size_t chunk_amount)
    {
        if(chunk_amount <= m_ChunkAmount)
        {
            // just shrink the buffer size instead of reallocating the buffer.
            m_BufferSize = chunk_amount * m_ChunkSize;
            // Clear with the new buffer size
            Clear();
            return;
        }
        
        byte* old_buffer = m_Buffer;
        
        size_t new_buffer_size = chunk_amount * m_ChunkSize;
        m_Buffer               = static_cast<byte*>(std::malloc(new_buffer_size));
        
        std::memcpy(m_Buffer, old_buffer, m_BufferSize);
        
        m_BufferSize  = new_buffer_size;
        m_ChunkAmount = chunk_amount;
        
        delete old_buffer;
        
        Clear();
    }
    
    size_t PoolAllocator::Capacity()
    {
        return m_BufferSize;
    }
} // namespace polos::memory