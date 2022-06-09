#pragma once

#ifndef POLOS_CORE_EVENTBUS_H_
#define POLOS_CORE_EVENTBUS_H_

#include "polos/events/event.h"
#include "polos/utils/concepts.h"
#include "polos/containers/containers.h"
#include "polos/containers/delegate.h"

namespace polos
{
    class EventBus
    {
        using EventSubscriber = Delegate<void(BaseEvent&)>;
    public:
        EventBus() noexcept = default;

        void Startup();
        void Shutdown();

        template<class event_type, typename... Args>
        static void RaiseEvent(Args&&... args);

        template<class event_type, class object_type, void(object_type::* method_ptr)(event_type&)>
        static void SubscribeToEvent(object_type *const ptr);

        template<class event_type, void(* const func_ptr)(event_type&)>
        static void SubscribeToEvent();
        
        template<class event_type>
        static void SubscribeToEvent(const Delegate<void(event_type &)>& cback);

        template<class event_type>
        static void UnsubscribeFromEvent(const Delegate<void(event_type &)>& cback);

    private:
        static EventBus* m_Instance;

        HashMap<StringId, DArray<EventSubscriber>> m_Callbacks;
    };
} // namespace polos

#define SUB_TO_EVENT(Event, Func)     ::polos::EventBus::SubscribeToEvent<Event, std::remove_cvref_t<decltype(*this)>, &std::remove_cvref_t<decltype(*this)>::Func>(this)
#define UNSUB_FROM_EVENT(Event, Func) ::polos::EventBus::UnsubscribeFromEvent(::polos::Delegate<void(Event&)>::template From<std::remove_cvref_t<decltype(*this)>, &std::remove_cvref_t<decltype(*this)>::Func>(this))

#include "event_bus.inl"

#endif /* POLOS_CORE_EVENTBUS_H_ */
