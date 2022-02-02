#pragma once
#ifndef POLOS_EVENTS_EVENT_H
#define POLOS_EVENTS_EVENT_H

#include "utils/types.h"

namespace polos
{
	static event_id id_counter = 1;

	class base_event
	{
		virtual event_id _id() = 0;
	};

	template<typename T>
	class event : base_event
	{
		virtual event_id _id() { return id; }
	public:
		friend class event_bus;
		virtual ~event() = default;
		inline static const event_id id = id_counter++;
	};
}


#endif /* POLOS_EVENTS_EVENT_H */
