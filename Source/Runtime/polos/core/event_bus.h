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

        template<EngineEvent event_type, typename... Args>
        static void RaiseEvent(Args&&... args);
        
        template<EngineEvent event_type>
        static void SubscribeToEvent(const Delegate<void(event_type &)>& cback);

        template<EngineEvent event_type>
        static void UnsubscribeFromEvent(const Delegate<void(event_type &)>& cback);

    private:
        static EventBus* m_Instance;

        HashMap<StringId, DArray<EventSubscriber>> m_Callbacks;
    };

    template<EngineEvent event_type, typename... Args>
    inline void EventBus::RaiseEvent(Args&&... args)
    {
        auto& cbs = m_Instance->m_Callbacks;
        if (cbs.contains(g_UniqueEventId<event_type>))
        {
            event_type e(std::forward<Args>(args)...);
            StringId id = g_UniqueEventId<event_type>;
            for (auto const& subscriber_function : cbs[id])
            {
                std::invoke(subscriber_function, e);
            }
        }
    }

    template<EngineEvent event_type>
    inline void EventBus::SubscribeToEvent(const Delegate<void(event_type&)>& cback)
    {
        auto& cbs = m_Instance->m_Callbacks;
        auto& sub = *std::launder(reinterpret_cast<EventSubscriber const*>(&cback));

        StringId id = g_UniqueEventId<event_type>;
        cbs.try_emplace(id).first->second.push_back(std::move(sub));
    }

    template<EngineEvent event_type>
    inline void EventBus::UnsubscribeFromEvent(const Delegate<void(event_type&)>& cback)
    {
        StringId id      = g_UniqueEventId<event_type>;
        auto& cbs        = m_Instance->m_Callbacks;
        auto& event_list = cbs.at(id);
        auto& sub        = *std::launder(reinterpret_cast<EventSubscriber const*>(&cback));
        event_list.erase(
            std::remove(
                event_list.begin(),
                event_list.end(),
                sub),
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
