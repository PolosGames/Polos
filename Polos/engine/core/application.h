#pragma once

#ifndef POLOS_CORE_APPLICATION_H
#define POLOS_CORE_APPLICATION_H

#include <memory>

#include "core/window.h"
#include "events/window/window_close.h"
#include "events/input/key_release.h"
#include "events/input/key_repeat.h"
#include "events/input/mouse_scroll.h"

namespace polos
{
	class application
	{
	public:
		application();
		virtual ~application();

		void run();
		void on_window_close(window_close& e);
		void on_key(key_release &e);
		void on_key_r(key_repeat &e);
		void on_mouse(mouse_scroll &e);
	private:
		std::unique_ptr<window> _window_instance;
		bool _is_running;
	};

	application *create_application();
}

#endif /* POLOS_CORE_APPLICATION_H */

