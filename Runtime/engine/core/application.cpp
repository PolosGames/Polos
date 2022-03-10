#include "plpch.h"

#include "event_bus.h"
#include "events/events.h"
#include "memory/linear_allocator.h"
#include "time/scope_timer.h"

#include "application.h"

namespace polos
{
	Application::Application()
		: m_IsRunning(true)
	{
		m_WindowInstance = std::unique_ptr<Window>(Window::NewWindow());
		EventBus::SubscribeToEvent<window_close, Application, &Application::on_window_close>(this);
	}
	
	Application::~Application()
	{}

	void Application::Run()
	{
		while (m_IsRunning)
		{
			m_WindowInstance->Update();
		}
	}
	
	void Application::on_window_close(window_close &e)
	{
		m_IsRunning = false;
		m_WindowInstance->Shutdown();
	}
} // namespace polos