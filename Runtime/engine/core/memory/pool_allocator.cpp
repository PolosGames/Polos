#include "debug/plassert.h"

#include "pool_allocator.h"

namespace polos::memory
{
    PoolAllocator::PoolAllocator()
        :m_ChunkSize{}, m_ChunkAmount{}
    {
        iBuffer = {nullptr, 0};
    }

	PoolAllocator::PoolAllocator(size_t chunk_size, size_t chunk_amount)
	{
		Initialize(chunk_size, chunk_amount);
	}

	PoolAllocator::~PoolAllocator()
	{
	    if(iBuffer.buffer != nullptr)
		    std::free(iBuffer.buffer);
		iBuffer.buffer       = nullptr;
		m_FreeListHead = nullptr;
	}
    
    PoolAllocator::PoolAllocator(PoolAllocator&& other) noexcept
        : iBuffer       (std::exchange(other.iBuffer, {nullptr, 0})),
          m_FreeListHead(std::exchange(other.m_FreeListHead, nullptr)),
          m_ChunkSize   (std::exchange(other.m_ChunkSize, 0)),
          m_ChunkAmount (std::exchange(other.m_ChunkAmount, 0))
    {
    
    }
    
    PoolAllocator& PoolAllocator::operator=(PoolAllocator&& rhs) noexcept
    {
        if(rhs.iBuffer.buffer == this->iBuffer.buffer) return *this;
        iBuffer        = std::exchange(rhs.iBuffer, {nullptr, 0});
        m_FreeListHead = std::exchange(rhs.m_FreeListHead, nullptr);
        m_ChunkSize    = std::exchange(rhs.m_ChunkSize, 0);
        m_ChunkAmount  = std::exchange(rhs.m_ChunkAmount, 0);
        return *this;
    }

	void PoolAllocator::Initialize(size_t chunk_size, size_t chunk_amount)
	{
		PROFILE_FUNC();
		iBuffer.bufferSize = chunk_size * chunk_amount;
		m_ChunkSize        = chunk_size;
		m_ChunkAmount      = chunk_amount;

		ASSERT(m_ChunkSize > sizeof(free_node) && iBuffer.bufferSize > sizeof(free_node));

		iBuffer.buffer     = static_cast<byte*>(std::malloc(iBuffer.bufferSize));

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
		ASSERT(!(iBuffer.buffer <= ptr && ptr < iBuffer.buffer + iBuffer.bufferSize));

		free_node* node = new (ptr) free_node{ m_FreeListHead };
		m_FreeListHead  = node;
	}

	void PoolAllocator::Clear()
	{
		PROFILE_FUNC();
		memset(iBuffer.buffer, 0, iBuffer.bufferSize);

		m_FreeListHead = new (&iBuffer.buffer[0]) free_node{ nullptr };
		auto* itr      = m_FreeListHead;

		for (size_t i = m_ChunkSize; i < iBuffer.bufferSize; i += m_ChunkSize)
		{
			auto* node = new (&iBuffer.buffer[i]) free_node{ nullptr };
			itr->next  = node;
			itr        = node;
		}
	}
    
    byte* PoolAllocator::Data() const
    {
        return iBuffer.buffer;
    }
    
    void PoolAllocator::Resize(size_t chunk_amount)
    {
        if(chunk_amount <= m_ChunkAmount)
        {
            // just shrink the buffer size instead of reallocating the buffer.
            iBuffer.bufferSize = chunk_amount * m_ChunkSize;
            // Clear with the new buffer size
            Clear();
            return;
        }
        
        byte* old_buffer = iBuffer.buffer;
        
        size_t new_buffer_size = chunk_amount * m_ChunkSize;
        iBuffer.buffer         = static_cast<byte*>(std::malloc(new_buffer_size));
        
        std::memcpy(iBuffer.buffer, old_buffer, iBuffer.bufferSize);
        
        iBuffer.bufferSize  = new_buffer_size;
        m_ChunkAmount       = chunk_amount;
        
        delete old_buffer;
        
        Clear();
    }
    
    size_t PoolAllocator::Capacity() const
    {
        return iBuffer.bufferSize;
    }
} // namespace polos::memory