#pragma once

#ifndef POLOS_CORE_WINDOW_H
#define POLOS_CORE_WINDOW_H

#include "utils/types.h"
#include <string>

namespace polos
{
	struct window_props
	{
		std::string title = "Polos";
		int32 width = 1280;
		int32 height = 720;
		int32 refresh_rate = 60;
		bool vsync = true;
		bool fullscreen = false;
	};

	class window
	{
	public:
		window() = default;
		virtual ~window() = default;

		virtual void initialize() = 0;
		virtual void shutdown() = 0;
		virtual void destroy() = 0;

		virtual uint32 width() = 0;
		virtual uint32 height() = 0;

		virtual bool vsync() = 0;
		virtual void vsync(bool vsync) = 0;

		virtual void update() = 0;
		virtual bool is_open() = 0;

		static window *create_window(const window_props& props = window_props());
	};
}

#endif /* POLOS_CORE_WINDOW_H */
