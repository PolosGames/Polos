#pragma once

#include "polos/core/events/event.h"
#include "polos/utils/concepts.h"
#include "polos/containers/containers.h"
#include "polos/containers/delegate.h"

namespace polos
{
    template<class T>
    concept EngineEvent = std::is_base_of_v<polos::Event<T>, T>;

    class EventBus
    {
    public:
        EventBus();

        void Startup();
        void Shutdown();

        template<EngineEvent EventType, typename... Args>
        static void RaiseEvent(Args&&... p_Args);
        
        template<EngineEvent EventType>
        static bool SubscribeToEvent(Delegate<void(EventType&)> p_Callback);

        template<EngineEvent EventType>
        static bool UnsubscribeFromEvent(Delegate<void(EventType&)> p_Callback);

    private:
        static EventBus* s_Instance;

        HashMap<StringId, DArray<Delegate<void(BaseEvent&)>>> m_Callbacks;
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
    inline bool EventBus::SubscribeToEvent(Delegate<void(EventType&)> p_Callback)
    {
        auto& callbacks  = s_Instance->m_Callbacks;

        StringId id = g_UniqueEventId<EventType>;
        callbacks.try_emplace(id).first->second.push_back(std::move(*std::launder(reinterpret_cast<Delegate<void(BaseEvent&)>*>(&p_Callback))));

        return true;
    }

    template<EngineEvent EventType>
    inline bool EventBus::UnsubscribeFromEvent(Delegate<void(EventType&)> p_Callback)
    {
        StringId id      = g_UniqueEventId<EventType>;
        auto& callbacks  = s_Instance->m_Callbacks;
        auto& event_list = callbacks.at(id);

        // Check for the same delegate in the subscribers, then delete it
        event_list.erase(
            std::remove(
                event_list.begin(),
                event_list.end(),
                *std::launder(reinterpret_cast<Delegate<void(BaseEvent&)>*>(&p_Callback))
            ),
            event_list.end()
        );

        return true;
    }
} // namespace polos

#define SUB_TO_EVENT_MEM_FUN(EventType, MemFuncName)     bool PL_ANON_NAME(result) = ::polos::EventBus::SubscribeToEvent(::polos::Delegate<void(EventType&)>::template From<std::remove_cvref_t<decltype(*this)>,&std::remove_cvref_t<decltype(*this)>::MemFuncName>(this)); PL_VOID_CAST(PL_ANON_NAME(result))
#define SUB_TO_EVENT_FREE_FUN(EventType, FuncPtr)        bool PL_ANON_NAME(result) = ::polos::EventBus::SubscribeToEvent(::polos::Delegate<void(EventType&)>::template From<FuncPtr>()); PL_VOID_CAST(PL_ANON_NAME(result))

#define UNSUB_FROM_EVENT_MEM_FUN(EventType, MemFuncName) bool PL_ANON_NAME(result) = ::polos::EventBus::UnsubscribeFromEvent(::polos::Delegate<void(EventType&)>::template From<std::remove_cvref_t<decltype(*this)>,&std::remove_cvref_t<decltype(*this)>::MemFuncName>(this)); PL_VOID_CAST(PL_ANON_NAME(result));
#define UNSUB_FROM_EVENT_FREE_FUN(EventType, FuncPtr)    bool PL_ANON_NAME(result) = ::polos::EventBus::UnsubscribeFromEvent(::polos::Delegate<void(EventType&)>::template From<FuncPtr>()); PL_VOID_CAST(PL_ANON_NAME(result))
