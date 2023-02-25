#pragma once

#include "polos/time/timer.h"

namespace polos::time
{
    enum class ScopeTimerUnit : uint8_t
    {
        kSecond,
        kMilliSecond,
        kMicroSecond,
    };

    class ScopeTimer
    {
    public:
        ScopeTimer(cstring scope_name);
        ScopeTimer(cstring scope_name, ScopeTimerUnit unit);
        ~ScopeTimer();

        void Stop();
        void Reset();
    private:
        void calculate_den(ScopeTimerUnit unit);
    private:
        float			m_Nom;
        cstring			m_UnitName;
        int64			m_Start;
        cstring			m_ScopeName;
        bool			m_IsStopped;
    };
}
