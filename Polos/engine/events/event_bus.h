#pragma once

#ifndef POLOS_EVENTS_EVENTBUS_H
#define POLOS_EVENTS_EVENTBUS_H

#include <unordered_map>

#include "containers/delegate.h"
#include "event.h"

namespace polos
{
	class event_bus
	{
	public:

		template<class event_type, typename... Args>
		static void raise_event(Args&& ...args)
		{
			if (_instance._callbacks.count(event_type::id))
			{
				event_type e(args...);
				for (delegate<void(base_event &)> subscriber_function : _instance._callbacks[event_type::id])
				{
					subscriber_function(e);
				}
			}
		}

		template<class event_type>
		static void subscribe_to_event(const delegate<void(event_type &)> &cback)
		{
			auto &cbs = _instance._callbacks;
			event_id id = event_type::id;
			cbs.try_emplace(id).first->second.push_back(reinterpret_cast<const delegate<void(base_event &)> &>(cback));
		}

		template<class event_type>
		static void subscribe_to_event(delegate<void(event_type &)> &&cback)
		{
			auto &cbs = _instance._callbacks;
			event_id id = event_type::id;
			cbs.try_emplace(id).first->second.push_back(reinterpret_cast<delegate<void(base_event &)> &&>(cback));
		}

		template<class event_type>
		static void unsubscribe_from_event(const delegate<void(event_type &)> &cback)
		{
			auto &cbs = _instance._callbacks;
			event_id id = event_type::id;
			cbs.at(id).erase(std::remove(cbs[id].begin(), cbs[id].end(), reinterpret_cast<const delegate<void(base_event &)> &>(cback)), cbs[id].end());
		}

		static event_bus &instance()
		{
			static event_bus e;
			return e;
		}
	private:

		static event_bus &_instance;
		std::unordered_map<event_id, std::vector<delegate<void(base_event &)>>> _callbacks;
	};
}

#endif /* POLOS_EVENTS_EVENTBUS_H */
