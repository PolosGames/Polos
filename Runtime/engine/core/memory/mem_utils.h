#pragma once
#ifndef POLOS_CORE_MEMORY_MEMUTILS_H_
#define POLOS_CORE_MEMORY_MEMUTILS_H_

#include "utils/alias.h"

namespace polos::memory
{
	class MemUtils
	{
	public:
		static uint32 constexpr MEM_ALIGNMENT = 2 * sizeof(void*);

		static bool IsPowerOfTwo(uintptr p);
		static uintptr AlignForward(uintptr p);
		static bool IsAligned(uintptr p);
		static uint32 CalculatePadding(uintptr p);
	};

	inline bool MemUtils::IsPowerOfTwo(uintptr p)
	{
		return (p & (p - 1)) == 0;
	}

	inline uintptr MemUtils::AlignForward(uintptr p)
	{
		uintptr misalignment = p & (MEM_ALIGNMENT - 1);
		uintptr aligned_ptr = p + MEM_ALIGNMENT - misalignment;

		return misalignment == 0 ? p : aligned_ptr;
	}

	inline bool MemUtils::IsAligned(uintptr p)
	{
		return (p & (MEM_ALIGNMENT - 1)) == 0;
	}

	inline uint32 MemUtils::CalculatePadding(uintptr p)
	{
		return MEM_ALIGNMENT - (p & (MEM_ALIGNMENT - 1));
	}
} // namespace polos

#endif /* POLOS_CORE_MEMORY_MEMUTILS_H_ */