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
	private:
		int64 start_;
		const char* scope_name_;
		bool is_stopped_;
	};
}

#endif /* POLOS_CORE_TIME_SCOPETIMER_H_ */