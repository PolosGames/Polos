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
		std::free(buffer_);
		buffer_			= nullptr;
		free_list_head_ = nullptr;
	}

	void PoolAllocator::Initialize(size_t chunk_size, size_t chunk_amount)
	{
		buffer_size_	= chunk_size * chunk_amount;
		chunk_size_		= chunk_size;

		ASSERT(chunk_size > sizeof(free_node*));

		buffer_			= (byte*)std::malloc(buffer_size_);
		free_list_head_	= (free_node*)buffer_;

		Clear();
	}

	void* PoolAllocator::Get()
	{
		free_node* node = free_list_head_;

		if (!node)
		{
			ASSERT_S(0, "PoolAllocator is out of memory.");
			return nullptr;
		}

		free_list_head_ = free_list_head_->next;

		return static_cast<void*>(node);
	}

	void PoolAllocator::Free(void* ptr)
	{
		ASSERT(!(buffer_ <= ptr && ptr < buffer_ + buffer_size_));

		free_node* node = (free_node*)ptr;
		node->next = free_list_head_;
		free_list_head_ = node;
	}

	void PoolAllocator::Clear()
	{
		for (size_t i = 0; i < buffer_size_; i += chunk_size_)
		{
			void* ptr = &buffer_[i];
			free_node* node = (free_node*)ptr;
			node->next = free_list_head_;
			free_list_head_ = node;
		}
	}
} // namespace polos::memory