#pragma once

#include "polos/core/events/event.h"

namespace polos
{
	struct key_repeat final : public Event<key_repeat>
	{
        int32 key{};
        int32 repeatCount{};
	};
}
