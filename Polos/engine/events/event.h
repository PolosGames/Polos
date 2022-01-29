#pragma once
#ifndef POLOS_EVENTS_EVENT_H
#define POLOS_EVENTS_EVENT_H

#include "utils/types.h"

namespace polos
{
	static event_id id_counter = 1;

	struct base_event
	{
		virtual ~base_event() = default;
		virtual event_id _id() = 0;
	};

	template<typename T>
	struct event : base_event
	{
		friend class event_bus;
		virtual ~event() = default;
		inline static const event_id id = id_counter++;
	private:
		virtual event_id _id()
		{
			return id;
		}
	};
}


#endif /* POLOS_EVENTS_EVENT_H */
