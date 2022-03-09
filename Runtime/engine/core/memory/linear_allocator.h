#pragma once
#ifndef POLOS_CORE_MEMORY_LINEARALLOCATOR_H_
#define POLOS_CORE_MEMORY_LINEARALLOCATOR_H_

#include "utils/macro_util.h"
#include "utils/alias.h"
#include "utils/feature.h"
#include "debug/profiling.h"

namespace polos::memory
{
	class LinearAllocator
	{
	public:
		PL_RULE_OF_FIVE_NO_DTOR(LinearAllocator)

		LinearAllocator(uint64 size);
		~LinearAllocator();

		void* Allocate(uint64 size);

		template<typename T, typename = typename std::enable_if_t<std::is_default_constructible_v<T>>>
		T* New();

		template<typename T, typename... Args>
		T* New(Args&&... args);

		template<typename T>
		T* NewArr(uint64 count);

		template<typename T>
		void Delete(T* ptr);

		template<typename T>
		void DeleteArr(T* ptr);

		void Resize(uint64 size);
		void Clear();
	private:
		PL_NODISCARD
		void* align(uint64 size);

	private:
		byte*      m_Buffer;
		uintptr    m_Bottom;
		uint64     m_Offset;
		uint64     m_BufferSize;
		std::mutex m_BufferMutex;
	};

	template<typename T, typename>
	inline T* LinearAllocator::New()
	{
		return new (align(sizeof(T))) T();
	}

	template<typename T, typename... Args>
	inline T* LinearAllocator::New(Args&&... args)
	{
		PROFILE_FUNC();
		return new (align(sizeof(T))) T(std::forward<Args>(args)...);
	}

	template<typename T>
	inline T* LinearAllocator::NewArr(uint64 count)
	{
		PROFILE_FUNC();
		return new (align(sizeof(T) * count)) T[count];
	}

	template<typename T>
	inline void LinearAllocator::Delete(T* ptr)
	{
		delete ptr;
	}

	template<typename T>
	inline void LinearAllocator::DeleteArr(T* ptr)
	{
		delete[] ptr;
	}
} // namespace polos

#endif /* POLOS_CORE_MEMORY_LINEARALLOCATOR_H_ */