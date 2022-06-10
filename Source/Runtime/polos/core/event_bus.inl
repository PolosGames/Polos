namespace polos
{
    template<EngineEvent event_type, typename... Args>
    inline void EventBus::RaiseEvent(Args&&... args)
    {
        auto& cbs = m_Instance->m_Callbacks;
        if (cbs.contains(event_type::id))
        {
            event_type e(std::forward<Args>(args)...);
            StringId id = event_type::id;
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

        StringId id = event_type::id;
        cbs.try_emplace(id).first->second.push_back(std::move(sub));
    }

    template<EngineEvent event_type>
    inline void EventBus::UnsubscribeFromEvent(const Delegate<void(event_type&)>& cback)
    {
        StringId id      = event_type::id;
        auto& cbs        = m_Instance->m_Callbacks;
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