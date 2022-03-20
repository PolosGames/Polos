#include "polos_pch.h"

#include "event_bus.h"

namespace polos
{
	EventBus* EventBus::m_Instance;

	void EventBus::Startup()
	{
		m_Instance = this;
	}

	void EventBus::Shutdown()
	{
		m_Instance = nullptr;
	}
}