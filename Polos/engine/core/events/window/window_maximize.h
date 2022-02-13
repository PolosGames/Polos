#pragma once

#ifndef POLOS_EVENTS_WINDOW_WINDOWMAXIMIZE_H
#define POLOS_EVENTS_WINDOW_WINDOWMAXIMIZE_H

#include "events/event.h"

namespace polos
{
	class window_maximize : public event<window_maximize>
	{
	public:
		int32 maximized;

		window_maximize() = default;
		window_maximize(int32 maximized) : maximized(maximized) {}
	};
}

#endif /* POLOS_EVENTS_WINDOW_WINDOWMAXIMIZE_H */
