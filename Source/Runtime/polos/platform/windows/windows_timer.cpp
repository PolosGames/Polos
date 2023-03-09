#ifdef POLOS_WIN

#include "polos/core/time/timer.h"

namespace polos::time
{
    int64 Timer::s_Freq;

    void Timer::Startup()
    {
        LARGE_INTEGER freq;
        BOOL ret = QueryPerformanceFrequency(&freq);
        ASSERT(ret, "QueryPerformanceFrequency didn't work.");

        s_Freq = freq.QuadPart;
    }

    int64 Timer::Now()
    {
        LARGE_INTEGER counter;

        BOOL ret = QueryPerformanceCounter(&counter);
        ASSERT(ret, "QueryPerformanceCounter didn't work.");

        // steady_clock's implementation.
        const int64 whole = (counter.QuadPart / s_Freq) * Period::den;
        const int64 part  = (counter.QuadPart % s_Freq) * Period::den / s_Freq;

        return whole + part;
    }
} // namespace polos::core::time

#endif // POLOS_WIN
