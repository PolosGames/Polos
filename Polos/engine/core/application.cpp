#include "plpch.h"
#include "application.h"

#include "core/event_bus.h"

namespace polos
{
	application::application()
		:_is_running(true)
	{
		_window_instance = std::unique_ptr<window>(window::create_window());
		event_bus::subscribe_to_event<window_close, application, &application::on_window_close>(this);
		event_bus::subscribe_to_event<key_release, application, &application::on_key>(this);
		event_bus::subscribe_to_event<key_repeat, application, &application::on_key_r>(this);
		event_bus::subscribe_to_event<mouse_scroll, application, &application::on_mouse>(this);
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

	void application::on_key(key_release &e)
	{
		LOG_INFO("KeyRelease: {0}", e.key);
	}

	void application::on_key_r(key_repeat &e)
	{
		LOG_INFO("KeyRepeat: {0} {1}", e.key, e.repeat_count);
	}

	void application::on_mouse(mouse_scroll &e)
	{
		LOG_INFO("MouseScroll: {0} {1}", e.x_offset, e.y_offset);
	}
}