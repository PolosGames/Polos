#pragma once

#ifndef POLOS_CORE_EVENTBUS_H_
#define POLOS_CORE_EVENTBUS_H_

#include <unordered_map>

#include "containers/delegate.h"
#include "events/event.h"

namespace polos
{
	class EventBus
	{
		using EventSubscriber = Delegate<void(base_event&)>;
	public:

		template<class event_type, typename... Args>
		static void raise_event(Args&& ...args);

		template<class event_type, class object_type, void(object_type::* method_ptr)(event_type&)>
		static void subscribe_to_event(object_type *const ptr);

		template<class event_type>
		static void subscribe_to_event(const Delegate<void(event_type &)> &cback);

		template<class event_type>
		static void subscribe_to_event(Delegate<void(event_type &)> &&cback);

		template<class event_type>
		static void unsubscribe_from_event(const Delegate<void(event_type &)> &cback);

		static EventBus &instance()
		{
			static EventBus e;
			return e;
		}
	private:
		std::unordered_map<event_id, std::vector<EventSubscriber>> m_Callbacks;
	};

	template<class event_type, typename ...Args>
	inline void EventBus::raise_event(Args && ...args)
	{
		auto& cbs = instance().m_Callbacks;
		if (cbs.count(event_type::id))
		{
			event_type e(args...);
			event_id id = event_type::id;
			for (auto& subscriber_function : cbs[id])
			{
				subscriber_function(e);
			}
		}
	}

	template<class event_type, class object_type, void(object_type::* method_ptr)(event_type &)>
	inline void EventBus::subscribe_to_event(object_type* const ptr)
	{
		auto& cbs = instance().m_Callbacks;
		auto del = Delegate<void(event_type&)>::template from<object_type, method_ptr>(reinterpret_cast<object_type *>(ptr));
		event_id id = event_type::id;
		cbs.try_emplace(id).first->second.push_back(reinterpret_cast<const EventSubscriber> &>(cback));
	}

	template<class event_type>
	inline void EventBus::subscribe_to_event(const Delegate<void(event_type&)>& cback)
	{
		auto& cbs = instance().m_Callbacks;
		event_id id = event_type::id;
		cbs.try_emplace(id).first->second.push_back(reinterpret_cast<EventSubscriber const&>(cback));
	}

	template<class event_type>
	inline void EventBus::subscribe_to_event(Delegate<void(event_type&)>&& cback)
	{
		auto &cbs = instance().m_Callbacks;
		event_id id = event_type::id;
		cbs.try_emplace(id).first->second.push_back(reinterpret_cast<EventSubscriber>&&>(cback));
	}

	template<class event_type>
	inline void EventBus::unsubscribe_from_event(const Delegate<void(event_type&)>& cback)
	{
		auto &cbs = instance().m_Callbacks;
		event_id id = event_type::id;
		cbs.at(id).erase(std::remove(cbs[id].begin(), cbs[id].end(), reinterpret_cast<EventSubscriber const&>(cback)), cbs[id].end());
	}

} // namespace polos

#endif /* POLOS_CORE_EVENTBUS_H_ */
