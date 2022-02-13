#include "plpch.h"
#include "application.h"

#include "event_bus.h"

namespace polos
{
	Application::Application()
		: _is_running(true)
	{
		_window_instance = std::unique_ptr<Window>(Window::create_window());
		event_bus::subscribe_to_event<window_close, Application, &Application::on_window_close>(this);
		event_bus::subscribe_to_event<key_release, Application, &Application::on_key>(this);
		event_bus::subscribe_to_event<key_repeat, Application, &Application::on_key_r>(this);
		event_bus::subscribe_to_event<mouse_scroll, Application, &Application::on_mouse>(this);
	}
	
	Application::~Application()
	{}

	void Application::run()
	{
		while (_is_running)
		{
			_window_instance->update();
		}
	}
	
	void Application::on_window_close(window_close &e)
	{
		_is_running = false;
		_window_instance->shutdown();
	}

	void Application::on_key(key_release &e)
	{
		LOG_INFO("KeyRelease: {0}", e.key);
	}

	void Application::on_key_r(key_repeat &e)
	{
		LOG_INFO("KeyRepeat: {0} {1}", e.key, e.repeat_count);
	}

	void Application::on_mouse(mouse_scroll &e)
	{
		LOG_INFO("MouseScroll: {0} {1}", e.x_offset, e.y_offset);
	}
}