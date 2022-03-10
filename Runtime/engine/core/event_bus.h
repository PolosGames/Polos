#pragma once

#ifndef POLOS_CORE_EVENTBUS_H_
#define POLOS_CORE_EVENTBUS_H_

#include "containers/delegate.h"
#include "events/event.h"

namespace polos
{
	class EventBus
	{
		using EventSubscriber = Delegate<void(base_event&)>;
	public:
		EventBus() = default;
		~EventBus() = default;

		void Startup();
		void Shutdown();

		template<class event_type, typename... Args>
		static void RaiseEvent(Args&&... args);

		template<class event_type, class object_type, void(object_type::* method_ptr)(event_type&)>
		static void SubscribeToEvent(object_type *const ptr);
		
		template<class event_type>
		static void SubscribeToEvent(const Delegate<void(event_type &)>& cback);

		template<class event_type>
		static void SubscribeToEvent(Delegate<void(event_type &)> &&cback);

		template<class event_type>
		static void UnsubscribeFromEvent(const Delegate<void(event_type &)>& cback);

	private:
		static EventBus* m_Instance;

		HashMap<event_id, DArray<EventSubscriber>> m_Callbacks;
	};

	template<class event_type, typename ...Args>
	inline void EventBus::RaiseEvent(Args&&... args)
	{
		auto& cbs = m_Instance->m_Callbacks;
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
	inline void EventBus::SubscribeToEvent(object_type* const ptr)
	{
		auto& cbs = m_Instance->m_Callbacks;
		auto del = Delegate<void(event_type&)>::template from<object_type, method_ptr>(reinterpret_cast<object_type *>(ptr));
		event_id id = event_type::id;
		cbs.try_emplace(id).first->second.push_back(reinterpret_cast<const EventSubscriber&>(del));
	}

	template<class event_type>
	inline void EventBus::SubscribeToEvent(const Delegate<void(event_type&)>& cback)
	{
		auto& cbs = m_Instance->m_Callbacks;
		event_id id = event_type::id;
		cbs.try_emplace(id).first->second.push_back(reinterpret_cast<EventSubscriber const&>(cback));
	}

	template<class event_type>
	inline void EventBus::SubscribeToEvent(Delegate<void(event_type&)>&& cback)
	{
		auto &cbs = m_Instance->m_Callbacks;
		event_id id = event_type::id;
		cbs.try_emplace(id).first->second.push_back(reinterpret_cast<EventSubscriber&&>(cback));
	}

	template<class event_type>
	inline void EventBus::UnsubscribeFromEvent(const Delegate<void(event_type&)>& cback)
	{
		auto &cbs = m_Instance->m_Callbacks;
		event_id id = event_type::id;
		cbs.at(id).erase(std::remove(cbs.at(id).begin(), cbs.at(id).end(), reinterpret_cast<const EventSubscriber&>(cback)), cbs.at(id).end());
	}

} // namespace polos

#endif /* POLOS_CORE_EVENTBUS_H_ */
