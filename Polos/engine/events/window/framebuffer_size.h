#pragma once

#ifndef POLOS_EVENTS_WINDOW_FRAMEBUFFERSIZE_H
#define POLOS_EVENTS_WINDOW_FRAMEBUFFERSIZE_H

#include "utils/types.h"
#include "events/event.h"

namespace polos
{
	class framebuffer_size final : public event<framebuffer_size>
	{
		uint32 width, height;
	public:
		framebuffer_size() = default;
		framebuffer_size(uint32 width, uint32 height) : width(width), height(height) {}
	};
}

#endif /* POLOS_EVENTS_WINDOW_FRAMEBUFFERSIZE_H */
