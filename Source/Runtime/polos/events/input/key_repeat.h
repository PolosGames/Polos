#pragma once

#include "polos/events/event.h"

namespace polos
{
	class key_repeat : public Event<key_repeat>
	{
	public:
		int32 key;
		int32 repeat_count;

		key_repeat() = default;
		key_repeat(int32 key, int32 repeat_count) : key(key), repeat_count(repeat_count) {}
	};
}
