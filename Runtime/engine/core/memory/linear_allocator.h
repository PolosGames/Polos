#pragma once
#ifndef POLOS_CORE_MEMORY_LINEARALLOCATOR_H_
#define POLOS_CORE_MEMORY_LINEARALLOCATOR_H_

#include <mutex>

#include "mem_utils.h"
#include "utils/macro_util.h"
#include "utils/alias.h"
#include "utils/feature.h"
#include "debug/profiling.h"
#include "utils/concepts.h"

namespace polos::memory
{
	class LinearAllocator
	{
    public:
        InternalBuffer iBuffer;
	public:
	    LinearAllocator();
		explicit LinearAllocator(uint64 size);
		~LinearAllocator();
        
        LinearAllocator(LinearAllocator&& other) noexcept;
        LinearAllocator& operator=(LinearAllocator&& rhs) noexcept;
        PL_NO_COPY(LinearAllocator);

        void  Initialize(uint64 size);
		void* Allocate(uint64 size);

		template<typename T>
        requires IsDefaultConstructible<T>
        PL_NODISCARD T* New();

		template<typename T, typename... Args>
        PL_NODISCARD T* New(Args&&... args);

		template<typename T>
        PL_NODISCARD T* NewArr(uint64 count);

		template<typename T>
		void Delete(T* ptr);

		template<typename T>
		void DeleteArr(T* ptr);
		
		PL_NODISCARD byte*  Data() const;
		PL_NODISCARD size_t Capacity() const;

		void Resize(uint64 size);
		void Clear();
	private:
        PL_NODISCARD void* align(uint64 size);

	private:
		uintptr    m_Bottom;
		size_t     m_Offset;
		std::mutex m_BufferMutex;
    };
} // namespace polos

#include "linear_allocator.inl"

#endif /* POLOS_CORE_MEMORY_LINEARALLOCATOR_H_ */