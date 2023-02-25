#include "polos/polos_pch.h"

#ifdef POLOS_WIN

#include "polos/time/timer.h"

namespace polos::time
{
    int64 Timer::s_Freq;

    void Timer::Startup()
    {
        LARGE_INTEGER freq;
        BOOL ret = QueryPerformanceFrequency(&freq);
        ASSERTSTR(ret, "QueryPerformanceFrequency didn't work.");

        s_Freq = freq.QuadPart;
    }

    int64 Timer::Now()
    {
        LARGE_INTEGER counter;

        BOOL ret = QueryPerformanceCounter(&counter);
        ASSERTSTR(ret, "QueryPerformanceCounter didn't work.");

        // steady_clock's implementation.
        const int64 Whole = (counter.QuadPart / s_Freq) * Period::den;
        const int64 Part  = (counter.QuadPart % s_Freq) * Period::den / s_Freq;

        return Whole + Part;
    }
} // namespace polos::core::time

#endif // POLOS_WIN
