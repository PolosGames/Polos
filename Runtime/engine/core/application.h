#pragma once

#ifndef POLOS_CORE_APPLICATION_H_
#define POLOS_CORE_APPLICATION_H_

#include <memory>

#include "window.h"
#include "utils/alias.h"
#include "events/window/window_close.h"
#include "events/input/key_release.h"
#include "events/input/key_repeat.h"
#include "events/input/mouse_scroll.h"

namespace polos
{
	class Application
	{
	public:
		struct P
		{
			int x;
			int y;
			int64 a;
			int64 b;
			int64 c;

			P() = default;
			P(int x, int y, int64 a, int64 b, int64 c) : x(x), y(y), a(a), b(b), c(c) { }
		};

		Application();
		virtual ~Application();

		void run();
		void on_window_close(window_close& e);
		void on_key(key_release &e);
		void on_key_r(key_repeat &e);
		void on_mouse(mouse_scroll &e);
	private:
		std::unique_ptr<Window> window_instance_;
		bool is_running_;
	};

	Application* create_application();
} // namespace polos

#endif /* POLOS_CORE_APPLICATION_H_ */

