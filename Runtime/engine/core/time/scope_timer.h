#pragma once
#ifndef POLOS_CORE_TIME_SCOPETIMER_H_
#define POLOS_CORE_TIME_SCOPETIMER_H_

#include "clock.h"

namespace polos::time
{
	class ScopeTimer
	{
	public:
		ScopeTimer(const char* scope_name);
		~ScopeTimer();

		void Stop();
		void Reset();
	private:
		int64   m_Start;
		cstring scope_name_;
		bool    m_IsStopped;
	};
}

#endif /* POLOS_CORE_TIME_SCOPETIMER_H_ */