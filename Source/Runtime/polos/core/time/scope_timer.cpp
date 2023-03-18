#include "scope_timer.h"

#include "polos/core/time/timer.h"

namespace polos::time
{
    ScopeTimer::ScopeTimer(std::string_view p_ScopeName)
        : m_Nom{1.0f}
        , m_ScopeName{p_ScopeName}
        , m_IsStopped{}
        , m_Unit{ScopeTimerUnit::k_MilliSecond}
    {
        calculate_den(m_Unit);
        m_Start = Timer::Now();
    }

    ScopeTimer::ScopeTimer(std::string_view p_ScopeName, ScopeTimerUnit p_Unit)
        : m_Nom{1.0f}
        , m_ScopeName{p_ScopeName}
    {
        calculate_den(p_Unit);
        m_Start = Timer::Now();
    }

    ScopeTimer::~ScopeTimer()
    {
        if (!m_IsStopped) Stop();
    }

    void ScopeTimer::Stop()
    {
        int64 end   = Timer::Now();
        m_IsStopped = true;
        LOG_ENGINE_INFO("Scope \"{0}\" took: {1:.5} {2}", m_ScopeName.data(), static_cast<float>(end - m_Start) * m_Nom, m_UnitName.data());
    }
    
    void ScopeTimer::Reset()
    {
        Stop();
        m_IsStopped = false;
        m_Start     = Timer::Now();
    }

    void ScopeTimer::calculate_den(ScopeTimerUnit p_Unit)
    {
        switch (p_Unit)
        {
            // Timer is in nanos, so nom should be other way around.
        case ScopeTimerUnit::k_Second:		m_Nom = 1.0_us; m_UnitName = "s"; break;
        case ScopeTimerUnit::k_MilliSecond:	m_Nom = 1.0_ms; m_UnitName = "ms";   break;
        case ScopeTimerUnit::k_MicroSecond:	m_Nom = 1.0f;   m_UnitName = "us";   break;
        }
    }
} // namespace polos::time