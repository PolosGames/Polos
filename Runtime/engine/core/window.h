#pragma once

#ifndef POLOS_CORE_WINDOW_H
#define POLOS_CORE_WINDOW_H

#include <string>

#include "types.h"
#include "system.h"

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

	class Window : public System<Window>
	{
	public:
		Window() = default;
		virtual ~Window() = default;

		virtual void Initialize() = 0;
		virtual void Shutdown() = 0;
		virtual void Destroy() = 0;

		virtual uint32 Width() = 0;
		virtual uint32 Height() = 0;

		virtual bool Vsync() = 0;
		virtual void Vsync(bool vsync) = 0;

		virtual void Update() = 0;
		virtual bool IsOpen() = 0;

		static Window* NewWindow(const window_props& props = window_props());
	};
}

#endif /* POLOS_CORE_WINDOW_H */
