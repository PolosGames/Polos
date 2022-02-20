#include "plpch.h"
#include "application.h"

#include "event_bus.h"
#include <optick.h>
#include <memory/stack_allocator.h>
#include "memory/linear_allocator.h"
#include "time/clock.h"
#include "events/events.h"

namespace polos
{
	Application::Application()
		: is_running_(true)
	{
		window_instance_ = std::unique_ptr<Window>(Window::NewWindow());
		event_bus::subscribe_to_event<window_close, Application, &Application::on_window_close>(this);
		event_bus::subscribe_to_event<key_release, Application, &Application::on_key>(this);
		event_bus::subscribe_to_event<key_repeat, Application, &Application::on_key_r>(this);
		event_bus::subscribe_to_event<mouse_scroll, Application, &Application::on_mouse>(this);
	}
	
	Application::~Application()
	{}

	void Application::run()
	{
		int64 start, end;
		//BEGIN_PROFILING();
		{
			//PROFILE_FUNC("Linear");
			memory::LinearAllocator s((sizeof(P)) * 10000);
			start = core::time::Clock::Now();
			for (int i = 0; i < 10000; i++) s.New<P>(i, i, i, i, i);
			end = core::time::Clock::Now();
		}
		LOG_CORE_INFO("LINEAR TIME: {0}", (end - start));
		{
			//PROFILE_FUNC("Malloc");
			P* ptr = (P*)std::malloc(sizeof(P) * 10000);
			
			start = core::time::Clock::Now();
			for (int i = 0; i < 10000; i++)
			{
				//PROFILE_SCOPE("Malloc:NEW");
				new (ptr + i) P(i, i, i, i, i);
			}
			end = core::time::Clock::Now();
			std::free(ptr);
		}
		LOG_CORE_INFO("malloc TIME: {0}", (end - start));

		//STOP_PROFILING();
		//SAVE_PROFILING("DENEME");
		while (is_running_)
		{
			window_instance_->Update();
		}
	}
	
	void Application::on_window_close(window_close &e)
	{
		is_running_ = false;
		window_instance_->Shutdown();
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
} // namespace polos