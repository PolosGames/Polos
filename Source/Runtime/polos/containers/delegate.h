#pragma once

/**
    Main concept is From Sergey Ryazanov's "The Impossibly Fast C++ Delegates"
    thread in the codeproject.com

    Currently, there is no type safety in template functions, but it should be
    added.
*/
#include "polos/utils/macro_util.h"
#include "polos/utils/concepts.h"

namespace polos
{
    template<typename Fn> class Delegate {};

    /*namespace detail
    {
        template<typename R, typename T, typename... Args>
        concept Lambda = requires {
            std::is_invocable_v<R, T, Args...>;
            !std::is_same_v<T, Delegate<R(Args...)>>;
        };
    }*/

    template<typename Return, typename... Args>
    class Delegate<Return(Args...)>
    {
    private:
        
        using ReturnType = Return;
        using StubType = ReturnType(*)(void*, Args&&...);
        
        using FreeFuncPtr = ReturnType(* const)(Args...);

        template<typename T>
        using MethodPtr = ReturnType(T::* const)(Args...);

        template<typename T>
        using ConstMethodPtr = ReturnType(T::* const)(Args...) const;

    public: // constructors
        
        PL_RULE_OF_FIVE(Delegate)

    public: 
        // Factory methods
        
        template<FreeFuncPtr ffunc_ptr>
        static Delegate From() noexcept
        {
            return { nullptr, function_stub<ffunc_ptr> };
        }
        
        template<class ObjType, MethodPtr<ObjType> method_ptr>
        static Delegate From(ObjType* const p_ObjPtr) noexcept
        {
            return { p_ObjPtr, method_stub<ObjType, method_ptr> };
        }
        
        template <typename ObjType, MethodPtr<ObjType> method_ptr>
        static Delegate From(ObjType& p_Object) noexcept
        {
            return { &p_Object, method_stub<ObjType, method_ptr> };
        }

        template<class ObjType, ConstMethodPtr<ObjType> method_ptr>
        static Delegate From(ObjType const* const p_ObjPtr) noexcept
        {
            return {const_cast<ObjType*>(&p_ObjPtr), const_method_stub<ObjType, method_ptr> };
        }

        template <typename ObjType, ConstMethodPtr<ObjType> method_ptr>
        static Delegate From(ObjType const& p_Object) noexcept
        {
            return { const_cast<ObjType*>(&p_Object), const_method_stub<ObjType, method_ptr> };
        }

        static Delegate From(ReturnType(* const p_FuncPtr)(Args...))
        {
            return p_FuncPtr;
        }

    public: // operators

        bool operator==(Delegate const& p_Other) const noexcept
        {
            return (m_ObjectPointer == p_Other.m_ObjectPointer) && (m_StubPointer == p_Other.m_StubPointer);
        }

        bool operator!=(Delegate const& p_Other) const noexcept
        {
            return *this != p_Other;
        }

        bool operator<(Delegate const& p_Other) const noexcept
        {
            return (m_ObjectPointer < p_Other.m_ObjectPointer)
                || ((m_ObjectPointer == p_Other.m_ObjectPointer) && (m_StubPointer < p_Other.m_StubPointer));
        }

        bool operator==(std::nullptr_t const) const noexcept
        {
            return !m_StubPointer;
        }

        bool operator!=(std::nullptr_t const) const noexcept
        {
            return m_StubPointer;
        }

        auto operator()(Args&&... p_Args) const noexcept
        {
            if constexpr (std::is_same_v<ReturnType, void>)
            {
                if (m_StubPointer) m_StubPointer(m_ObjectPointer, std::forward<Args>(p_Args)...);
            }
            else
            {
                return m_StubPointer(m_ObjectPointer, std::forward<Args>(p_Args)...);
            }
        }
    private:
        template <FreeFuncPtr ffunc_ptr>
        static ReturnType function_stub(void* const, Args&&... p_Args)
        {
            return ffunc_ptr(std::forward<Args>(p_Args)...);
        }

        template <typename ObjType, MethodPtr<ObjType> method_ptr>
        static ReturnType method_stub(void* const p_ObjectPtr, Args&&... p_Args)
        {
            return (static_cast<ObjType*>(p_ObjectPtr)->*method_ptr)(std::forward<Args>(p_Args)...);
        }

        template <typename ObjType, ConstMethodPtr<ObjType> method_ptr>
        static ReturnType const_method_stub(void* const p_ObjectPtr, Args&&... p_Args)
        {
            return (reinterpret_cast<ObjType const*>(p_ObjectPtr)->*method_ptr)(std::forward<Args>(p_Args)...);
        }

    private:
        Delegate(void* const p_ObjectPtr, StubType const p_StubPtr) noexcept
            : m_ObjectPointer(p_ObjectPtr), m_StubPointer(p_StubPtr)
        {}
    private:
        void*    m_ObjectPointer;
        StubType m_StubPointer;
    };
}
