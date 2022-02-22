#include "plpch.h"

#ifdef POLOS_WIN

#include "time/clock.h"

namespace polos::time
{
	LARGE_INTEGER Clock::Freq;
	
	void Clock::Initialize()
	{
		BOOL ret = QueryPerformanceFrequency(&Clock::Freq);
		ASSERT_S(ret, "QueryPerformanceFrequency didn't work.");
	}

	int64 Clock::Now()
	{
		LARGE_INTEGER Counter;

		BOOL ret = QueryPerformanceCounter(&Counter);
		ASSERT_S(ret, "QueryPerformanceCounter didn't work.");

		// steady_clock's implementation.
		const int64 Whole = (Counter.QuadPart / Freq.QuadPart) * period::den;
		const int64 Part = (Counter.QuadPart % Freq.QuadPart) * period::den / Freq.QuadPart;

		return Whole + Part;
	}
} // namespace polos::core::time

#endif // POLOS_WIN
