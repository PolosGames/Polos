#pragma once

#ifndef POLOS_CORE_APPLICATION_H
#define POLOS_CORE_APPLICATION_H

namespace polos
{
	class application
	{
	public:
		application();
		virtual ~application();

		void run();
	};

	application *create_application();
}

#endif /* POLOS_CORE_APPLICATION_H */

