
#include "update_queue.h"

namespace polos
{
    UpdateQueue* UpdateQueue::s_Instance;

    void UpdateQueue::PutLast(FuncType p_UpdateFunc)
    {
        s_Instance->m_UpdateList.push_back(std::move(p_UpdateFunc));
    }

    void UpdateQueue::PutFirst(FuncType p_UpdateFunc)
    {
        s_Instance->m_UpdateList.insert(s_Instance->m_UpdateList.cbegin(), std::forward<FuncType>(p_UpdateFunc));
    }

    void UpdateQueue::Update(float p_DeltaTime)
    {
        for(const auto& func: s_Instance->m_UpdateList)
        {
            std::invoke(func, std::forward<float>(p_DeltaTime));
        }
    }
}// namespace polos