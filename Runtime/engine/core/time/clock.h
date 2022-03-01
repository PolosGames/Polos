#pragma once
#ifndef POLOS_CORE_TIME_CLOCK_H_
#define POLOS_CORE_TIME_CLOCK_H_

#include "utils/alias.h"
#include "utils/macro_util.h"
#include "engine/subsytem.h"
#include "engine/engine.h"

namespace polos::time
{
	class Clock : public Subsystem<Clock>
	{
		inline static Clock& m_Instance = Subsystem<Clock>::Get();
		using Period = std::micro;
	public:
		Clock() = default;
		DELETE_COPY_MOVE_CTOR(Clock)

		static void  OnInitialize();
		static int64 Now();
	private:
		static int64 m_Freq;
	};
	
	SubscribeToEngineInit(Clock, &Clock::Initialize);
	SubscribeToEngineShutdown(Clock, &Clock::Shutdown);
} // namespace polos::core::time

#endif /* POLOS_CORE_TIME_CLOCK_H_ */