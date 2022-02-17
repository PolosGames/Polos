#pragma once
#ifndef POLOS_CORE_MEMORY_MEMUTILS_H_
#define POLOS_CORE_MEMORY_MEMUTILS_H_

#include "types.h"

namespace polos::memory
{
	class MemUtils
	{
	public:
		static uint32 constexpr MEM_ALIGNMENT = 8;

		static bool IsPowerOfTwo(uintptr p);
		static uintptr AlignForward(uintptr p);
		static uint32 CalculatePadding(uintptr p);
	};

	inline bool MemUtils::IsPowerOfTwo(uintptr p)
	{
		return (p & (p - 1)) == 0;
	}

	inline uintptr MemUtils::AlignForward(uintptr p)
	{
		uintptr misalignment = p & (MEM_ALIGNMENT - 1);

		return misalignment == 0 ? p : p + MEM_ALIGNMENT - misalignment;
	}

	inline uint32 MemUtils::CalculatePadding(uintptr p)
	{
		return MEM_ALIGNMENT - (p & (MEM_ALIGNMENT - 1));
	}
} // namespace polos

#endif /* POLOS_CORE_MEMORY_MEMUTILS_H_ */