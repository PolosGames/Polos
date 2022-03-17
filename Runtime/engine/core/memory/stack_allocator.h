#pragma once
#ifndef POLOS_CORE_MEMORY_STACKALLOCATOR_H_
#define POLOS_CORE_MEMORY_STACKALLOCATOR_H_

#include "utils/macro_util.h"
#include "utils/alias.h"
#include "utils/feature.h"
#include "debug/profiling.h"

namespace polos::memory
{
	class StackAllocator
	{
		struct stack_header
		{
			uint64 prev_offset; /// Stores previous element's header's start
		};
	public:
		explicit StackAllocator(uint64 size);
		~StackAllocator();
		
        PL_DELETE_COPY_MOVE_CTOR(StackAllocator)

		template<typename T, typename... Args>
		T* Push(Args&&... args);

		template<typename T>
		T* PushArr(uint64 count);
		
		byte* Data();

		void Pop();
		void Clear();
	private:
		PL_NODISCARD
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