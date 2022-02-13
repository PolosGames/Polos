#pragma once

#ifndef POLOS_EVENTS_INPUT_MOUSESCROLL_H
#define POLOS_EVENTS_INPUT_MOUSESCROLL_H

#include "events/event.h"

namespace polos
{
	class mouse_scroll : public event<mouse_scroll>
	{
	public:
		float x_offset, y_offset;

		mouse_scroll() = default;
		mouse_scroll(float x_offset, float y_offset) : x_offset(x_offset), y_offset(y_offset) {}
	};
}

#endif /* POLOS_EVENTS_INPUT_MOUSESCROLL_H */
