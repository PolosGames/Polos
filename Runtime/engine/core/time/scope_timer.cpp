#include "plpch.h"
#include "scope_timer.h"

namespace polos::time
{
	ScopeTimer::ScopeTimer(const char* scope_name)
		: scope_name_(scope_name), is_stopped_(false)
	{
		start_ = Clock::Now();
	}

	ScopeTimer::~ScopeTimer()
	{
		if (!is_stopped_) Stop();
	}

	void ScopeTimer::Stop()
	{
		int64 end = Clock::Now();
		LOG_CORE_INFO("Scope \"{0}\" took: {1} ns", scope_name_, (end - start_));
		is_stopped_ = true;
	}
} // namespace polos::time