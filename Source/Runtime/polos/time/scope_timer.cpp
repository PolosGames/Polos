#include "polos/polos_pch.h"

#include "scope_timer.h"

namespace polos::time
{
    ScopeTimer::ScopeTimer(const char* scope_name)
        : m_Nom(1.0f), m_UnitName("ms"),
          m_ScopeName(scope_name), m_IsStopped(false)
    {
        calculate_den(ScopeTimerUnit::kMilliSecond);
        m_Start = Timer::Now();
    }

    ScopeTimer::ScopeTimer(const char* scope_name, ScopeTimerUnit unit)
        : m_Nom(1.0f),
          m_ScopeName(scope_name), m_IsStopped(false)
    {
        calculate_den(unit);
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
        LOG_ENGINE_INFO("Scope \"{0}\" took: {1:.5} {2}", m_ScopeName, static_cast<float>(end - m_Start) * m_Nom, m_UnitName);
    }
    
    void ScopeTimer::Reset()
    {
        Stop();
        m_IsStopped = false;
        m_Start     = Timer::Now();
    }

    void ScopeTimer::calculate_den(ScopeTimerUnit unit)
    {
        switch (unit)
        {
            // Timer is in nanos, so nom should be other way around.
        case ScopeTimerUnit::kSecond:		m_Nom = 1.0_us; m_UnitName = "secs";	break;
        case ScopeTimerUnit::kMilliSecond:	m_Nom = 1.0_ms; m_UnitName = "ms";	break;
        case ScopeTimerUnit::kMicroSecond:	m_Nom = 1.0f; m_UnitName = "us";	break;
        }
    }
} // namespace polos::time