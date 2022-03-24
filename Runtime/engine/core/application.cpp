#include "polos_pch.h"

#include "event_bus.h"
#include "events/events.h"

#include "application.h"

namespace polos
{
	Application::Application()
		: m_IsRunning(true)
	{
		m_WindowInstance = std::unique_ptr<IWindow>(IWindow::NewWindow());
		EventBus::SubscribeToEvent<window_close, Application, &Application::on_window_close>(this);
	}

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