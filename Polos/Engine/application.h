#pragma once

#include "core.h"

namespace polos
{
	class POLOSAPI application
	{
	public:
		application();
		virtual ~application();

		void run();
	};

	application *create_application();
}

