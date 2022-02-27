#pragma once

#ifndef POLOS_EVENTS_EVENTBUS_H
#define POLOS_EVENTS_EVENTBUS_H

#include <unordered_map>

#include "containers/delegate.h"
#include "events/event.h"

namespace polos
{
	class EventBus
	{
	public:

		template<class event_type, typename... Args>
		static void raise_event(Args&& ...args);

		template<class event_type, class object_type, void(object_type::* method_ptr)(event_type&)>
		static void subscribe_to_event(object_type *const ptr);

		template<class event_type>
		static void subscribe_to_event(const delegate<void(event_type &)> &cback);

		template<class event_type>
		static void subscribe_to_event(delegate<void(event_type &)> &&cback);

		template<class event_type>
		static void unsubscribe_from_event(const delegate<void(event_type &)> &cback);

		static EventBus &instance()
		{
			static EventBus e;
			return e;
		}
	private:
		std::unordered_map<event_id, std::vector<delegate<void(base_event &)>>> m_Callbacks;
	};

	template<class event_type, typename ...Args>
	inline void EventBus::raise_event(Args && ...args)
	{
		if (instance().m_Callbacks.count(event_type::id))
		{
			event_type e(args...);
			event_id id = event_type::id;
			for (auto &subscriber_function : instance().m_Callbacks[id])
			{
				subscriber_function(e);
			}
		}
	}

	template<class event_type, class object_type, void(object_type::* method_ptr)(event_type &)>
	inline void EventBus::subscribe_to_event(object_type * const ptr)
	{
		auto del = delegate<void(event_type&)>::template from<object_type, method_ptr>(reinterpret_cast<object_type *>(ptr));
		subscribe_to_event<event_type>(del);
	}

	template<class event_type>
	inline void EventBus::subscribe_to_event(const delegate<void(event_type&)>& cback)
	{
		auto &cbs = instance().m_Callbacks;
		event_id id = event_type::id;
		cbs.try_emplace(id).first->second.push_back(reinterpret_cast<const delegate<void(base_event &)> &>(cback));
	}

	template<class event_type>
	inline void EventBus::subscribe_to_event(delegate<void(event_type&)>&& cback)
	{
		auto &cbs = instance().m_Callbacks;
		event_id id = event_type::id;
		cbs.try_emplace(id).first->second.push_back(reinterpret_cast<delegate<void(base_event &)> &&>(cback));
	}

	template<class event_type>
	inline void EventBus::unsubscribe_from_event(const delegate<void(event_type&)>& cback)
	{
		auto &cbs = instance().m_Callbacks;
		event_id id = event_type::id;
		cbs.at(id).erase(std::remove(cbs[id].begin(), cbs[id].end(), reinterpret_cast<const delegate<void(base_event &)> &>(cback)), cbs[id].end());
	}

}

#endif /* POLOS_EVENTS_EVENTBUS_H */
