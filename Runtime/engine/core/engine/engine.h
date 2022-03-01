#pragma once

#ifndef POLOS_CORE_ENGINE_H_
#define POLOS_CORE_ENGINE_H_

#include <unordered_set>

#include "containers/delegate.h"
#include "utils/alias.h"
#include "utils/macro_util.h"

namespace polos
{
	class Engine
	{
		using Subscriber = void(*)(void);
	public:
		static void Run();
		static void Stop();

		static byte SubscribeToInitSeq(Subscriber fptr);
		static byte SubscribeToShutdownSeq(Subscriber fptr);

		static Engine& Instance()
		{
			static Engine e;
			return e;
		}
	private:
		std::unordered_set<Subscriber> m_InitSeq;
		std::unordered_set<Subscriber> m_ShutdownSeq;
	};

#define SubscribeToEngineInit(Type, Function) inline static byte PL_ANON_NAME(PL_CONCAT(Type, OnInitialize)) = Engine::SubscribeToInitSeq(Function);
#define SubscribeToEngineShutdown(Type, Function) inline static byte PL_ANON_NAME(PL_CONCAT(Type, OnShutdown)) = Engine::SubscribeToShutdownSeq(Function);
	
	inline byte Engine::SubscribeToInitSeq(Subscriber fptr)
	{
		auto& init_seq = Instance().m_InitSeq;
		
		if (init_seq.contains(fptr)) return 0;

		init_seq.insert(fptr);

		return 1;
	}

	inline byte Engine::SubscribeToShutdownSeq(Subscriber fptr)
	{
		auto& shut_seq = Instance().m_ShutdownSeq;

		if (shut_seq.contains(fptr)) return 0;

		shut_seq.insert(fptr);
		return 1;
	}
} // namespace polos

#endif /* POLOS_CORE_ENGINE_H_ */