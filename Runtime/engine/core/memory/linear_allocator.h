#pragma once
#ifndef POLOS_CORE_MEMORY_LINEARALLOCATOR_H_
#define POLOS_CORE_MEMORY_LINEARALLOCATOR_H_

#include "utils/macro_util.h"

namespace polos::memory
{
	class LinearAllocator
	{
	public:
		PL_RULE_OF_FIVE_NO_DTOR(LinearAllocator)

		LinearAllocator(uint64 size);
		~LinearAllocator();

		template<typename T, typename... Args>
		T* New(Args&&... args);

		template<typename T>
		T* NewArr(uint64 count);

		void Resize(uint64 size);
		void Clear();
	private:
		void* align(uint64 size);

	private:
		byte*      m_Buffer;
		uintptr    m_Bottom;
		uint64     m_Offset;
		uint64     m_BufferSize;
		std::mutex m_BufferMutex;
	};

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
} // namespace polos

#endif /* POLOS_CORE_MEMORY_LINEARALLOCATOR_H_ */