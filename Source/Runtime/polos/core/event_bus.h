#pragma once

#ifndef POLOS_CORE_EVENTBUS_H_
#define POLOS_CORE_EVENTBUS_H_

#include "polos/events/event.h"
#include "polos/utils/concepts.h"
#include "polos/containers/containers.h"
#include "polos/containers/delegate.h"

namespace polos
{
    template<class T>
    concept EngineEvent = std::is_base_of_v<polos::Event<T>, T>;

    class EventBus
    {
        using EventSubscriber = Delegate<void(BaseEvent&)>;
    public:
        EventBus() noexcept = default;

        void Startup();
        void Shutdown();

        template<EngineEvent event_type, typename... Args>
        static void RaiseEvent(Args&&... args);
        
        template<EngineEvent event_type>
        static void SubscribeToEvent(const Delegate<void(event_type &)>& cback);

        template<EngineEvent event_type>
        static void UnsubscribeFromEvent(const Delegate<void(event_type &)>& cback);

    private:
        static EventBus* s_Instance;

        HashMap<StringId, DArray<EventSubscriber>> m_Callbacks;
    };
} // namespace polos

#define SUB_TO_EVENT_MEM_FUN(EventType, MemFuncName) \
    ::polos::EventBus::SubscribeToEvent( \
        ::polos::Delegate<void(EventType&)>::template From< \
            std::remove_cvref_t<decltype(*this)>, \
            &std::remove_cvref_t<decltype(*this)>::MemFuncName \
        >(this) \
    )

#define SUB_TO_EVENT_FREE_FUN(EventType, FuncPtr) \
    ::polos::EventBus::SubscribeToEvent( \
        ::polos::Delegate<void(EventType&)>::template From<FuncPtr>() \
    )

#define UNSUB_FROM_EVENT_MEM_FUN(EventType, MemFuncName) \
    ::polos::EventBus::UnsubscribeFromEvent( \
        ::polos::Delegate<void(EventType&)>::template From< \
            std::remove_cvref_t<decltype(*this)>, \
            &std::remove_cvref_t<decltype(*this)>::MemFuncName \
        >(this) \
    )

#define UNSUB_FROM_EVENT_FREE_FUN(EventType, FuncPtr) \
    ::polos::EventBus::UnsubscribeFromEvent(          \
            ::polos::Delegate<void(EventType&)>::template From<FuncPtr>() \
    )

#include "event_bus.inl"

#endif /* POLOS_CORE_EVENTBUS_H_ */
