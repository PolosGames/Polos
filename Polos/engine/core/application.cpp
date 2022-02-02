#include "plpch.h"
#include "application.h"

namespace polos
{
	application::application()
	{
		_window_instance = std::unique_ptr<window>(window::create_window());
	}
	
	application::~application()
	{}

	void application::run()
	{
		while (_window_instance->is_open())
		{
			_window_instance->update();
		}
	}
}