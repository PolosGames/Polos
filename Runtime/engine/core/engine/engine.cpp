#include "plpch.h"

#include "engine.h"

namespace polos
{
	void Engine::Run()
	{
		auto& init_sequence = Instance().m_InitSeq;
		for (Subscriber subber : init_sequence)
		{
			subber();
		}
	}
	
	void Engine::Stop()
	{
		auto& shut_seq = Instance().m_ShutdownSeq;
		for (Subscriber subber : shut_seq)
		{
			subber();
		}
	}
} // namespace polos
