namespace polos
{
    template<class event_type, typename ...Args>
    inline void EventBus::RaiseEvent(Args&&... args)
    {
        auto& cbs = m_Instance->m_Callbacks;
        if (cbs.contains(event_type::id))
        {
            event_type e(std::forward<Args>(args)...);
            StringId id = event_type::id;
            for (auto const& subscriber_function : cbs[id])
            {
                subscriber_function(e);
            }
        }
    }
    
    template<class event_type, class object_type, void(object_type::* method_ptr)(event_type &)>
    inline void EventBus::SubscribeToEvent(object_type* const ptr)
    {
        auto& cbs = m_Instance->m_Callbacks;
        
        auto  del = Delegate<void(event_type&)>::template From<object_type, method_ptr>(ptr);
        auto* sub = std::launder(reinterpret_cast<EventSubscriber*>(&del));

        StringId id = event_type::id;
        cbs.try_emplace(id).first->second.push_back(std::move(*sub));
    }

    template<class event_type, void (*const func_ptr)(event_type&)>
    inline void EventBus::SubscribeToEvent()
    {
        auto& cbs = m_Instance->m_Callbacks;
        
        auto  del = Delegate<void(event_type&)>::template From<func_ptr>();
        auto* sub = std::launder(reinterpret_cast<EventSubscriber*>(&del));

        StringId id  = event_type::id;
        cbs.try_emplace(id).first->second.push_back(std::move(*sub));
    }

    template<class event_type>
    inline void EventBus::SubscribeToEvent(const Delegate<void(event_type&)>& cback)
    {
        auto& cbs   = m_Instance->m_Callbacks;
        auto* sub = std::launder(reinterpret_cast<EventSubscriber*>(&cback));

        StringId id = event_type::id;
        cbs.try_emplace(id).first->second.push_back(std::move(*sub));
    }

    template<class event_type>
    inline void EventBus::UnsubscribeFromEvent(const Delegate<void(event_type&)>& cback)
    {
        StringId id = event_type::id;
        auto& cbs        = m_Instance->m_Callbacks;
        auto& event_list = cbs.at(id);
        auto* sub        = std::launder(reinterpret_cast<EventSubscriber*>(&cback));
        std::remove(
            event_list.begin(),
            event_list.end(),
            *sub
        );
    }
}