#pragma once

#ifndef POLOS_CORE_EVENTBUS_H_
#define POLOS_CORE_EVENTBUS_H_

#include "events/event.h"
#include "utils/concepts.h"
#include "containers/containers.h"
#include "containers/delegate.h"

namespace polos
{
    class EventBus
    {
        using EventSubscriber = Delegate<void(base_event&)>;
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

        HashMap<EventId, DArray<EventSubscriber>> m_Callbacks;
    };
} // namespace polos

#include "event_bus.inl"

#endif /* POLOS_CORE_EVENTBUS_H_ */
