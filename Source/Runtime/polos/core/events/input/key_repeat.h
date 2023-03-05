#pragma once

#include "polos/events/event.h"

namespace polos
{
	class key_repeat final : public Event<key_repeat>
	{
	public:
		key_repeat() = default;
        explicit key_repeat(int32 p_Key, int32 p_RepatCount) : key{p_Key}, repeatCount{p_RepatCount} {}
	public:
        int32 key{};
        int32 repeatCount{};
	};
}
