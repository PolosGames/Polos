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
	    PoolAllocator();
		explicit PoolAllocator(size_t chunk_size, size_t chunk_amount);
		~PoolAllocator();
		
		PoolAllocator(PoolAllocator&& other)          noexcept;
		PoolAllocator& operator=(PoolAllocator&& rhs) noexcept;
		
		PL_NO_COPY(PoolAllocator);

		void Initialize(size_t chunk_size, size_t chunk_amount);
		void Resize(size_t chunk_amount);

		PL_NODISCARD
		void* GetNextFree();
        
        size_t Capacity();
        byte* Data();
        
        void Free(void* ptr);
        void Clear();
    private:
		byte*      m_Buffer;
		free_node* m_FreeListHead;
		size_t     m_BufferSize;
		size_t     m_ChunkSize;
        size_t     m_ChunkAmount;
	};

	// Templated pool allocator
	template<typename T>
	class TPoolAllocator
	{
	public:
	    TPoolAllocator() = default;
        explicit TPoolAllocator(size_t count);
        
        TPoolAllocator(TPoolAllocator&& other) noexcept;
        TPoolAllocator& operator=(TPoolAllocator&& rhs) noexcept;
        
        void Initialize(size_t count);

		template<typename ...Args>
		T* New(Args&&... args);
        
        PL_NODISCARD size_t Capacity();
        PL_NODISCARD size_t ByteCapacity();
        
        T* Data();

		void Free(T* ptr);
	private:
		PoolAllocator m_PoolAllocator;
	};
} // namespace polos::memory

#include "pool_allocator.inl"

#endif /* POLOS_CORE_MEMORY_POOLALLOCATOR_H_ */