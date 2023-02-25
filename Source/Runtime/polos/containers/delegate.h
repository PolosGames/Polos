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
        static Delegate From(ObjType* const obj_ptr) noexcept
        {
            return { obj_ptr, method_stub<ObjType, method_ptr> };
        }
        
        template <typename ObjType, MethodPtr<ObjType> method_ptr>
        static Delegate From(ObjType& object) noexcept
        {
            return { &object, method_stub<ObjType, method_ptr> };
        }

        template<class ObjType, ConstMethodPtr<ObjType> method_ptr>
        static Delegate From(ObjType const* const obj_ptr) noexcept
        {
            return {const_cast<ObjType*>(&obj_ptr), const_method_stub<ObjType, method_ptr> };
        }

        template <typename ObjType, ConstMethodPtr<ObjType> method_ptr>
        static Delegate From(ObjType const& object) noexcept
        {
            return { const_cast<ObjType*>(&object), const_method_stub<ObjType, method_ptr> };
        }

        static Delegate From(ReturnType(* const f_ptr)(Args...))
        {
            return f_ptr;
        }
#pragma region operators
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
                if (m_StubPointer) m_StubPointer(m_ObjectPointer, std::forward<Args>(args)...);
            }
            else
            {
                return m_StubPointer(m_ObjectPointer, std::forward<Args>(args)...);
            }
        }
#pragma endregion

    private:
        template <FreeFuncPtr ffunc_ptr>
        static ReturnType function_stub(void* const, Args&&... args)
        {
            return ffunc_ptr(std::forward<Args>(args)...);
        }

        template <typename ObjType, MethodPtr<ObjType> method_ptr>
        static ReturnType method_stub(void* const object_ptr, Args&&... args)
        {
            return (static_cast<ObjType*>(object_ptr)->*method_ptr)(std::forward<Args>(args)...);
        }

        template <typename ObjType, ConstMethodPtr<ObjType> method_ptr>
        static ReturnType const_method_stub(void* const object_ptr, Args&&... args)
        {
            return (reinterpret_cast<ObjType const*>(object_ptr)->*method_ptr)(std::forward<Args>(args)...);
        }

    private:
        Delegate(void* const object_pointer, StubType const stub_ptr) noexcept
            : m_ObjectPointer(object_pointer), m_StubPointer(stub_ptr)
        {}
    private:
        void*    m_ObjectPointer;
        StubType m_StubPointer;
    };
}
