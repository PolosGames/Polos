#pragma once

#ifndef POLOS_EVENTS_EVENT_H
#define POLOS_EVENTS_EVENT_H

#include "utils/type_util.h"
#include "utils/alias.h"

namespace polos
{
	class base_event
	{
		virtual event_id _id() = 0;
	};

	template<typename T>
	class event : base_event
	{
		event_id _id() override { return id; }
	public:
		friend class event_bus;
		inline static const event_id id = type_hash<T>();
		
		virtual ~event() = default;
	};
}

#endif /* POLOS_EVENTS_EVENT_H */
