namespace polos
{
    template<class event_type, typename ...Args>
    inline void EventBus::RaiseEvent(Args&&... args)
    {
        auto& cbs = m_Instance->m_Callbacks;
        if (cbs.count(event_type::id))
        {
            event_type e(args...);
            EventId id = event_type::id;
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
        auto del = Delegate<void(event_type&)>::template From<object_type, method_ptr>(
                reinterpret_cast<object_type *>(ptr));
        EventId id = event_type::id;
        cbs.try_emplace(id).first->second.push_back(reinterpret_cast<EventSubscriber const&>(del));
    }

    template<class event_type, void (*const func_ptr)(event_type&)>
    inline void EventBus::SubscribeToEvent()
    {
        auto& cbs = m_Instance->m_Callbacks;
        auto del = Delegate<void(event_type&)>::template From<func_ptr>();
        EventId id = event_type::id;
        cbs.try_emplace(id).first->second.push_back(reinterpret_cast<EventSubscriber const&>(del));
    }

    template<class event_type>
    inline void EventBus::SubscribeToEvent(const Delegate<void(event_type&)>& cback)
    {
        auto& cbs = m_Instance->m_Callbacks;
        EventId id = event_type::id;
        cbs.try_emplace(id).first->second.push_back(reinterpret_cast<EventSubscriber const&>(cback));
    }
    
    template<class event_type>
    inline void EventBus::UnsubscribeFromEvent(const Delegate<void(event_type&)>& cback)
    {
        auto &cbs = m_Instance->m_Callbacks;
        EventId id = event_type::id;
        cbs.at(id).erase(std::remove(cbs.at(id).begin(), cbs.at(id).end(), reinterpret_cast<const EventSubscriber&>(cback)), cbs.at(id).end());
    }
}