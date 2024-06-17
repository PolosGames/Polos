#ifdef POLOS_LINUX

#include "polos/core/time/timer.h"

namespace polos::time
{
    void Timer::Startup()
    {}
    
    int64 Timer::Now()
    {
        auto now      = std::chrono::steady_clock::now().time_since_epoch();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(now);

        return duration.count();
    }
}

#endif /* POLOS_LINUX */