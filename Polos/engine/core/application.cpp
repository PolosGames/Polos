#include "plpch.h"
#include "application.h"

#include "events/event_bus.h"

namespace polos
{
	application::application()
		:_is_running(true)
	{
		_window_instance = std::unique_ptr<window>(window::create_window());
		event_bus::subscribe_to_event<window_close, application, &application::on_window_close>(this);
	}
	
	application::~application()
	{}

	void application::run()
	{
		while (_is_running)
		{
			_window_instance->update();
		}
	}
	void application::on_window_close(window_close &e)
	{
		_is_running = false;
		_window_instance->shutdown();
	}
}