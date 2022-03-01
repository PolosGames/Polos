#pragma once
#ifndef POLOS_CORE_TIME_CLOCK_H_
#define POLOS_CORE_TIME_CLOCK_H_

#include <ratio>

#include "utils/alias.h"

namespace polos::time
{
	class Clock
	{
		using Period = std::micro;
	public:
		static void  OnInitialize();
		static int64 Now();
	private:
		static int64 m_Freq;
	};
} // namespace polos::core::time

#endif /* POLOS_CORE_TIME_CLOCK_H_ */