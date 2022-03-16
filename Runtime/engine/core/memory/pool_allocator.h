#pragma once
#ifndef POLOS_CORE_MEMORY_POOLALLOCATOR_H_
#define POLOS_CORE_MEMORY_POOLALLOCATOR_H_

#include "utils/alias.h"
#include "utils/macro_util.h"
#include "utils/feature.h"
#include "debug/profiling.h"

namespace polos::memory
{
	class PoolAllocator
	{
		struct free_node
		{
			free_node* next;
		};
	public:
		PL_RULE_OF_FIVE_NO_DTOR(PoolAllocator)

		explicit PoolAllocator(size_t chunk_size, size_t chunk_amount);
		~PoolAllocator();

		void Initialize(size_t chunk_size, size_t chunk_amount);

		PL_NODISCARD
		void* GetNextFree();
        
        byte* Data();
        
        void Free(void* ptr);
        void Clear();
    private:
		byte*      m_Buffer;
		free_node* m_FreeListHead;
		size_t     m_BufferSize;
		size_t     m_ChunkSize;
	};

	// Templated pool allocator
	template<typename T>
	class TPoolAllocator
	{
	public:
		PL_RULE_OF_FIVE(TPoolAllocator)
		explicit TPoolAllocator(size_t count);

		template<typename ...Args>
		T* New(Args&&... args);

		void Free(T* ptr);
	private:
		PoolAllocator m_PoolAllocator;
	};

	template<typename T>
	TPoolAllocator<T>::TPoolAllocator(size_t count)
		: m_PoolAllocator(PoolAllocator(sizeof(T), count))
	{
	}

	template<typename T>
	template<typename ...Args>
	T* TPoolAllocator<T>::New(Args&&... args)
	{
		PROFILE_FUNC();
		return new (m_PoolAllocator.GetNextFree()) T(std::forward<Args>(args)...);
	}

	template<typename T>
	void TPoolAllocator<T>::Free(T* ptr)
	{
		PROFILE_FUNC();
		m_PoolAllocator.Free(ptr);
	}
} // namespace polos::memory

#endif /* POLOS_CORE_MEMORY_POOLALLOCATOR_H_ */