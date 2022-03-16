#pragma once

#ifndef POLOS_CONTEXT_GRAPHICSCONTEXT_H
#define POLOS_CONTEXT_GRAPHICSCONTEXT_H

#include "plconfig.h"

namespace polos
{
	class graphics_context
	{
	public:
		void Initialize(void* window_handle);
	public:
		bool is_initialized;
	private:
		void* m_window;
	};
}

#endif /* POLOS_CONTEXT_GRAPHICSCONTEXT_H */
