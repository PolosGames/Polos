#pragma once
#ifndef POLOS_CORE_CONTAINERS_DELEGATE_H_
#define POLOS_CORE_CONTAINERS_DELEGATE_H_

/**
    Main concept is From Sergey Ryazanov's "The Impossibly Fast C++ Delegates"
    thread in the codeproject.com

    Currently, there is no type safety in template functions, but it should be
    added.

    Most of this implementation is From Rythe-Interactive/LegionEngine repo.
*/

#include "utils/macro_util.h"
#include "utils/concepts.h"

namespace polos
{
    template <typename Function> class Delegate;
    
    template<typename Return, typename... Args>
    class Delegate<Return(Args...)>
    {
        using ReturnType = Return;
        using StubType = ReturnType(*)(void*, Args&&...);
        using FunctionPointer = ReturnType(*)(Args...);
        
        Delegate(void* const object_pointer, StubType const stub_ptr) noexcept
            : m_ObjectPointer(object_pointer), m_StubPointer(stub_ptr)
        {}
    public: // constructors
        
        PL_RULE_OF_FIVE(Delegate);
        
        template <typename owner_type>
        requires IsClass<owner_type>
        explicit Delegate(owner_type const* const o) noexcept
            : m_ObjectPointer(const_cast<owner_type*>(o))
        {}
        
        template <typename owner_type>
        requires IsClass<owner_type>
        explicit Delegate(owner_type const& o) noexcept
            : m_ObjectPointer(const_cast<owner_type*>(&o))
        {}
        
        template<typename FuncPtr>
        requires IsNotSame<typename std::decay_t<FuncPtr>, Delegate>
        Delegate(FuncPtr func_ptr)
        {
            m_ObjectPointer = nullptr;
            m_StubPointer = function_stub<typename std::decay_t<FuncPtr>>;
        }
        
    public: // Factory methods
        template <ReturnType(* const function_ptr)(Args...)>
        static Delegate From() noexcept
        {
            return { nullptr, function_stub<function_ptr> };
        }
        
        template<class owner_object, ReturnType(owner_object::* const method_ptr)(Args...)>
        static Delegate From(owner_object* const object_pointer) noexcept
        {
            return { object_pointer, method_stub<owner_object, method_ptr> };
        }
        
        template<class owner_object, ReturnType(owner_object:: *const method_ptr)(Args...) const>
        static Delegate From(owner_object const* const object_pointer) noexcept
        {
            return { object_pointer, const_method_stub<owner_object, method_ptr> };
        }
        
        template <typename owner_type, ReturnType(owner_type::* const method_ptr)(Args...)>
        static Delegate From(owner_type& object) noexcept
        {
            return { &object, method_stub<owner_type, method_ptr> };
        }
        
        template <typename owner_type, ReturnType(owner_type::* const method_ptr)(Args...) const>
        static Delegate From(owner_type const& object) noexcept
        {
            return { const_cast<owner_type*>(&object), const_method_stub<owner_type, method_ptr> };
        }
        
        static Delegate From(ReturnType(* const f_ptr)(Args...))
        {
            return f_ptr;
        }
        
    public: // operators
        
        bool operator==(Delegate const& other) const noexcept
        {
            return (m_ObjectPointer == other.m_ObjectPointer) && (m_StubPointer == other.m_StubPointer);
        }
        
        bool operator!=(Delegate const& other) const noexcept
        {
            return *this != other;
        }
        
        bool operator<(Delegate const& other) const noexcept
        {
            return (m_ObjectPointer < other.m_ObjectPointer) 
                || ((m_ObjectPointer == other.m_ObjectPointer) && (m_StubPointer < other.m_StubPointer));
        }
        
        bool operator==(std::nullptr_t const) const noexcept
        {
            return !m_StubPointer;
        }
        
        bool operator!=(std::nullptr_t const) const noexcept
        {
            return m_StubPointer;
        }
        
        auto operator()(Args&&... args) const noexcept
        {
            if constexpr (std::is_same_v<ReturnType, void>)
            {
                if(m_StubPointer) m_StubPointer(m_ObjectPointer, std::forward<Args>(args)...);
            }
            else
            {
                return m_StubPointer(m_ObjectPointer, std::forward<Args>(args)...);
            }
        }
    private:
        template <ReturnType(* function_ptr)(Args...)>
        static ReturnType function_stub(void* const, Args&&... args)
        {
            return function_ptr(std::forward<Args>(args)...);
        }
        
        template <typename owner_object, ReturnType(owner_object::* method_ptr)(Args...)>
        static ReturnType method_stub(void* const object_ptr, Args&&... args)
        {
            return (static_cast<owner_object*>(object_ptr)->*method_ptr)(std::forward<Args>(args)...);
        }
        
        template <typename owner_object, ReturnType(owner_object::* method_ptr)(Args...) const>
        static ReturnType const_method_stub(void* const object_ptr, Args&&... args)
        {
            return (static_cast<owner_object*>(object_ptr)->*method_ptr)(std::forward<Args>(args)...);
        }
        
    private:
        void* m_ObjectPointer;
        StubType m_StubPointer;
    };
}

#endif /* POLOS_CORE_CONTAINERS_DELEGATE_H_ */