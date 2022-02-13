#pragma once

#ifndef POLOS_CORE_SYSTEM_H
#define POLOS_CORE_SYSTEM_H

namespace polos
{
	template<typename >
	class System
	{
	public:
		virtual void initialize() = 0;
		virtual void shutdown() = 0;
	};
}

#endif /* POLOS_CORE_SYSTEM_H */
