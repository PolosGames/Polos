#include "plpch.h"

#ifdef POLOS_WIN

#include "core/time/clock.h"

namespace polos::time
{
	int64 Clock::Now()
	{
		LARGE_INTEGER Freq;
		LARGE_INTEGER Counter;

		{
			BOOL ret = QueryPerformanceFrequency(&Freq);
			ASSERT_S(ret, "QueryPerformanceFrequency didn't work.");
		}
		{
			BOOL ret = QueryPerformanceCounter(&Counter);
			ASSERT_S(ret, "QueryPerformanceCounter didn't work.");
		}

		const int64 Whole = (Counter.QuadPart / Freq.QuadPart) * period::den;
		const int64 Part = (Counter.QuadPart % Freq.QuadPart) * period::den / Freq.QuadPart;

		return Whole + Part;
	}
} // namespace polos::core::time

#endif // POLOS_WIN
