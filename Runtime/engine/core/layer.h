#pragma once

#ifndef POLOS_CORE_LAYER_H_
#define POLOS_CORE_LAYER_H_

namespace polos
{
	class layer
	{
	public:
		virtual void initialize() = 0;
	protected:
		bool _is_visible;
	};
}

#endif /* POLOS_CORE_LAYER_H_ */
