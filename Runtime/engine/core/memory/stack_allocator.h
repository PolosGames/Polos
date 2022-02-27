#pragma once
#ifndef POLOS_CORE_MEMORY_STACKALLOCATOR_H_
#define POLOS_CORE_MEMORY_STACKALLOCATOR_H_

#include "linear_allocator.h"
#include "utils/macro_util.h"

namespace polos::memory
{
	class StackAllocator
	{
		struct stack_header
		{
			uint64 prev_offset; /// Stores previous element's header's start
		};
	public:
		PL_RULE_OF_FIVE_NO_DTOR(StackAllocator)

		explicit StackAllocator(uint64 size);
		~StackAllocator();

		template<typename T, typename... Args>
		T* Push(Args&&... args);

		template<typename T>
		T* PushArr(uint64 count);

		void Pop();
		void Clear();
	private:
		void* align(uint64 size);
	private:
		byte*   m_Buffer;
		uintptr m_Bottom;
		uint64  m_PrevOffset;
		uint64  m_Offset;
		uint64  m_BufferSize;
	};

	template<typename T, typename... Args>
	inline T* StackAllocator::Push(Args&&... args)
	{
		PROFILE_FUNC();
		return new (align(sizeof(T))) T(std::forward<Args>(args)...);
	}

	template<typename T>
	inline T* StackAllocator::PushArr(uint64 count)
	{
		PROFILE_FUNC();
		return new (align(sizeof(T) * count)) T[count];
	}
} // namespace polos

#endif /* POLOS_CORE_MEMORY_STACKALLOCATOR_H_ */