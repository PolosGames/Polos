#pragma once

#ifndef POLOS_CORE_APPLICATION_H
#define POLOS_CORE_APPLICATION_H

#include "core/window.h"
#include <memory>

namespace polos
{
	class application
	{
	public:
		application();
		virtual ~application();

		void run();
	private:
		std::unique_ptr<window> _window_instance;
	};

	application *create_application();
}

#endif /* POLOS_CORE_APPLICATION_H */

