#pragma once

#ifndef POLOS_CORE_APPLICATION_H_
#define POLOS_CORE_APPLICATION_H_

#include <memory>

#include "window.h"
#include "utils/alias.h"
#include "events/window/window_close.h"

namespace polos
{
	class Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
	private:
		void on_window_close(window_close& e);
	private:
		std::unique_ptr<Window> m_WindowInstance;
		bool m_IsRunning;
	};

	Application* create_application();
} // namespace polos

#endif /* POLOS_CORE_APPLICATION_H_ */

