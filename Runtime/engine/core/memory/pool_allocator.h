#pragma once
#ifndef POLOS_CORE_MEMORY_POOLALLOCATOR_H_
#define POLOS_CORE_MEMORY_POOLALLOCATOR_H_

#include "utils/alias.h"

namespace polos::memory
{
	class PoolAllocator
	{
		struct free_node
		{
			free_node* next;
		};
	public:
		PoolAllocator() = default;
		PoolAllocator(size_t chunk_size, size_t chunk_amount);
		~PoolAllocator();

		void Initialize(size_t chunk_size, size_t chunk_amount);
		void* Get();
		void Free(void* ptr);
		void Clear();
	private:
		byte* buffer_;
		free_node* free_list_head_;
		size_t buffer_size_;
		size_t chunk_size_;
	};

	// Templated pool allocator
	template<typename T>
	class TPoolAllocator
	{
		TPoolAllocator() = default;
		TPoolAllocator(size_t count);

		template<typename ...Args>
		T* New(Args&&... args);

		void Free(T* ptr);
	private:
		PoolAllocator pool_allocator_;
	};

	template<typename T>
	template<typename ...Args>
	T* TPoolAllocator<T>::New(Args&&... args)
	{
		return new (pool_allocator_.Get()) T(std::forward<Args>(args)...);
	}

	template<typename T>
	void TPoolAllocator<T>::Free(T* ptr)
	{
		pool_alllocator_.Free(ptr);
	}
} // namespace polos::memory

#endif /* POLOS_CORE_MEMORY_POOLALLOCATOR_H_ */