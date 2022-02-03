#pragma once

#ifndef POLOS_EVENTS_INPUT_KEYRELEASE_H
#define POLOS_EVENTS_INPUT_KEYRELEASE_H

#include "events/event.h"

namespace polos
{
	class key_release : public event<key_release>
	{
	public:
		int32 key;

		key_release() = default;
		key_release(int32 key) : key(key) {}
	};
}

#endif /* POLOS_EVENTS_INPUT_KEYRELEASE_H */
