#pragma once
#ifndef POLOS_CORE_TIME_CLOCK_H_
#define POLOS_CORE_TIME_CLOCK_H_

#include <ratio>

#include "utils/alias.h"
#include "utils/macro_util.h"

namespace polos::time
{
	class Timer
	{
		using Period = std::micro;
	public:
		PL_DELETE_COPY_MOVE_CTOR(Timer)
	
		static void  OnStartUp();
	
		static int64 Now();
	private:
		static int64 m_Freq;
		
		Timer();
	};
} // namespace polos::core::time

#endif /* POLOS_CORE_TIME_CLOCK_H_ */