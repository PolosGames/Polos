#include "plpch.h"
#include "scope_timer.h"

namespace polos::time
{
	ScopeTimer::ScopeTimer(const char* scope_name)
		: m_ScopeName(scope_name), m_IsStopped(false)
	{
		m_Start = Clock::Now();
	}

	ScopeTimer::~ScopeTimer()
	{
		if (!m_IsStopped) Stop();
	}

	void ScopeTimer::Stop()
	{
		int64 end   = Clock::Now();
		m_IsStopped = true;
		LOG_CORE_INFO("Scope \"{0}\" took: {1} ns", m_ScopeName, (end - m_Start));
	}
	
	void ScopeTimer::Reset()
	{
		Stop();
		m_IsStopped = false;
		m_Start     = Clock::Now();
	}
} // namespace polos::time