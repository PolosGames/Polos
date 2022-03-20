#pragma once

#ifndef POLOS_CORE_APPLICATION_H_
#define POLOS_CORE_APPLICATION_H_

#include <memory>

#include "i_window.h"
#include "utils/alias.h"
#include "events/window/window_close.h"

namespace polos
{
	class Application
	{
	public:
		Application();

		void Run();
	private:
		void on_window_close(window_close& e);
	private:
		std::unique_ptr<IWindow> m_WindowInstance;
		bool                     m_IsRunning;
	};

	Application* CreateApplication(void* ptr);
} // namespace polos

#endif /* POLOS_CORE_APPLICATION_H_ */

