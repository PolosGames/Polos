#pragma once

#ifndef POLOS_CONTEXT_GRAPHICSCONTEXT_H
#define POLOS_CONTEXT_GRAPHICSCONTEXT_H

namespace polos
{
	class graphics_context
	{
	public:
		void initialize(void *window_handle);
	private:
		void *_window;
	};
}

#endif /* POLOS_CONTEXT_GRAPHICSCONTEXT_H */