#pragma once
#ifndef POLOS_CORE_UPDATEQUEUE_H_
#define POLOS_CORE_UPDATEQUEUE_H_

#include "containers/containers.h"
#include "containers/delegate.h"

namespace polos
{
    class UpdateQueue
    {
    public:
        using FuncType = Delegate<void(float)>;
    public:
        static void PutLast(FuncType&& update_func);
        static void PutFirst(FuncType&& update_func);
        static void Update(float delta_time);

    private:
        friend class Engine;
        static UpdateQueue* m_Instance;
        DArray<FuncType> m_UpdateList;
    };
} // namespace polos

#endif // POLOS_CORE_UPDATEQUEUE_H_