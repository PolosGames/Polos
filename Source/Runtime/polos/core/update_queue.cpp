#include "polos/polos_pch.h"

#include "update_queue.h"

namespace polos
{
    UpdateQueue* UpdateQueue::m_Instance;

    void UpdateQueue::PutLast(FuncType&& update_func)
    {
        m_Instance->m_UpdateList.push_back(std::move(update_func));
    }

    void UpdateQueue::PutFirst(FuncType&& update_func)
    {
        m_Instance->m_UpdateList.insert(m_Instance->m_UpdateList.cbegin(), std::forward<FuncType>(update_func));
    }

    void UpdateQueue::Update(float delta_time)
    {
        for(const auto& func: m_Instance->m_UpdateList)
        {
            std::invoke(func, std::forward<float>(delta_time));
        }
    }
}// namespace polos