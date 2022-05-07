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

/* 
 * Func in all the update queue macros is just the function name(without any symbols) 
*/ 

// these two macros should be called only from inside the member functions.
#define UPDATE_Q_MEM_ADD_LAST(Type, Func) ::polos::UpdateQueue::PutLast( \
    ::polos::UpdateQueue::FuncType::From<Type, &Type::Func>(this))
#define UPDATE_Q_MEM_ADD_FIRST(Type, Func) ::polos::UpdateQueue::PutFirst(\
    ::polos::UpdateQueue::FuncType::From<Type, &Type::Func>(this))

// this macro should be used with non-member functions
#define UPDATE_Q_NMEM_ADD_LAST(Func) ::polos::UpdateQueue::PutLast( \
        ::polos::UpdateQueue::FuncType::From<&Func>())
#define UPDATE_Q_NMEM_ADD_FIRST(Func) ::polos::UpdateQueue::PutFirst( \
        ::polos::UpdateQueue::FuncType::From<&Func>())

#endif // POLOS_CORE_UPDATEQUEUE_H_