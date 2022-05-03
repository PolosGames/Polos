#pragma once

#ifndef POLOS_EVENTS_INPUT_KEYREPEAT_H
#define POLOS_EVENTS_INPUT_KEYREPEAT_H

#include "events/event.h"

namespace polos
{
	class key_repeat : public event<key_repeat>
	{
	public:
		int32 key;
		int32 repeat_count;

		key_repeat() = default;
		key_repeat(int32 key, int32 repeat_count) : key(key), repeat_count(repeat_count) {}
	};
}


#endif /* POLOS_EVENTS_INPUT_KEYREPEAT_H */
