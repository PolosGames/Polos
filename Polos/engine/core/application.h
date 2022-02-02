#pragma once

#ifndef POLOS_CORE_APPLICATION_H
#define POLOS_CORE_APPLICATION_H

#include <memory>

#include "core/window.h"
#include "events/window/window_close.h"

namespace polos
{
	class application
	{
	public:
		application();
		virtual ~application();

		void run();
		void on_window_close(window_close& e);
	private:
		std::unique_ptr<window> _window_instance;
		bool _is_running;
	};

	application *create_application();
}

#endif /* POLOS_CORE_APPLICATION_H */

