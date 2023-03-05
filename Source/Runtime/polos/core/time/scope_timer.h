#pragma once

#include "polos/time/timer.h"

namespace polos::time
{
    enum class ScopeTimerUnit : uint8_t
    {
        k_Second,
        k_MilliSecond,
        k_MicroSecond,
    };

    class ScopeTimer
    {
    public:
        ScopeTimer(std::string_view p_ScopeName);
        ScopeTimer(std::string_view p_ScopeName, ScopeTimerUnit p_Unit);
        ~ScopeTimer();

        void Stop();
        void Reset();
    private:
        void calculate_den(ScopeTimerUnit unit);
    private:
        float            m_Nom{};
        std::string_view m_UnitName;
        ScopeTimerUnit   m_Unit;
        int64            m_Start{};
        std::string_view m_ScopeName;
        bool             m_IsStopped{};
    };
}
