#pragma once

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

        template<EngineEvent EventType, typename... Args>
        static void RaiseEvent(Args&&... p_Args);
        
        template<EngineEvent EventType>
        static void SubscribeToEvent(Delegate<void(EventType&)> p_Callback);

        template<EngineEvent EventType>
        static void UnsubscribeFromEvent(Delegate<void(EventType&)> p_Callback);

    private:
        static EventBus* s_Instance;

        HashMap<StringId, DArray<EventSubscriber>> m_Callbacks;
    };

    template<EngineEvent EventType, typename... Args>
    inline void EventBus::RaiseEvent(Args&&... p_Args)
    {
        auto& callbacks = s_Instance->m_Callbacks;
        if (callbacks.contains(g_UniqueEventId<EventType>))
        {
            EventType e(std::forward<Args>(p_Args)...);
            StringId id = g_UniqueEventId<EventType>;
            for (auto const& subscriber_function : callbacks[id])
            {
                std::invoke(subscriber_function, e);
            }
        }
    }

    template<EngineEvent EventType>
    inline void EventBus::SubscribeToEvent(Delegate<void(EventType&)> p_Callback)
    {
        auto& callbacks  = s_Instance->m_Callbacks;
        auto& subscriber = *std::launder(reinterpret_cast<EventSubscriber const*>(&p_Callback));

        StringId id = g_UniqueEventId<EventType>;
        callbacks.try_emplace(id).first->second.push_back(std::move(subscriber));
    }

    template<EngineEvent EventType>
    inline void EventBus::UnsubscribeFromEvent(Delegate<void(EventType&)> p_Callback)
    {
        StringId id      = g_UniqueEventId<EventType>;
        auto& callbacks  = s_Instance->m_Callbacks;
        auto& event_list = callbacks.at(id);
        auto& subscriber = *std::launder(reinterpret_cast<EventSubscriber*>(&p_Callback));
        event_list.erase(
            std::remove(
                event_list.begin(),
                event_list.end(),
                subscriber),
            event_list.end());
    }
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
