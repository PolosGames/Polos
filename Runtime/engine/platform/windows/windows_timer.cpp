#include "platform_detection.h"

#ifdef POLOS_WIN

#include "debug/plassert.h"
#include "time/timer.h"

namespace polos::time
{
	int64 Timer::m_Freq;

	void Timer::OnStartUp()
	{
		LARGE_INTEGER freq;
		BOOL ret = QueryPerformanceFrequency(&freq);
		ASSERTSTR(ret, "QueryPerformanceFrequency didn't work.");

		m_Freq = freq.QuadPart;
	}

	int64 Timer::Now()
	{
		LARGE_INTEGER Counter;

		BOOL ret = QueryPerformanceCounter(&Counter);
		ASSERTSTR(ret, "QueryPerformanceCounter didn't work.");

		// steady_clock's implementation.
		const int64 Whole = (Counter.QuadPart / m_Freq) * Period::den;
		const int64 Part = (Counter.QuadPart % m_Freq) * Period::den / m_Freq;

		return Whole + Part;
	}
} // namespace polos::core::time

#endif // POLOS_WIN
