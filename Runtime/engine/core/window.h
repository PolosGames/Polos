#pragma once

#ifndef POLOS_CORE_WINDOW_H
#define POLOS_CORE_WINDOW_H

#include <string>

#include "types.h"
#include "system.h"

namespace polos
{
	struct WindowProps
	{
		std::string title = "Polos";
		int32 width = 1280;
		int32 height = 720;
		int32 refresh_rate = 60;
		bool vsync = true;
		bool fullscreen = false;
	};

	class Window : public System<Window>
	{
	public:
		Window() = default;
		virtual ~Window() = default;

		virtual void initialize() = 0;
		virtual void shutdown() = 0;
		virtual void destroy() = 0;

		virtual uint32 width() = 0;
		virtual uint32 height() = 0;

		virtual bool vsync() = 0;
		virtual void vsync(bool vsync) = 0;

		virtual void update() = 0;
		virtual bool is_open() = 0;

		static Window* create_window(const WindowProps& props = WindowProps());
	};
}

#endif /* POLOS_CORE_WINDOW_H */
