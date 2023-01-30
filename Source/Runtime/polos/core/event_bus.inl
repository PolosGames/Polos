namespace polos
{
    template<EngineEvent event_type, typename... Args>
    inline void EventBus::RaiseEvent(Args&&... args)
    {
        auto& cbs = s_Instance->m_Callbacks;
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
        auto& cbs = s_Instance->m_Callbacks;
        auto& sub = *std::launder(reinterpret_cast<EventSubscriber const*>(&cback));

        StringId id = g_UniqueEventId<event_type>;
        cbs.try_emplace(id).first->second.push_back(std::move(sub));
    }

    template<EngineEvent event_type>
    inline void EventBus::UnsubscribeFromEvent(const Delegate<void(event_type&)>& cback)
    {
        StringId id = g_UniqueEventId<event_type>;
        auto& cbs        = s_Instance->m_Callbacks;
        auto& event_list = cbs.at(id);
        auto& sub        = *std::launder(reinterpret_cast<EventSubscriber const*>(&cback));
        event_list.erase(
            std::remove(
                event_list.begin(),
                event_list.end(),
                sub
            ),
            event_list.end()
        );
    }
}