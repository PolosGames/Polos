#pragma once
#ifndef POLOS_CONTAINERS_DELEGATE_H
#define POLOS_CONTAINERS_DELEGATE_H

/**
	Main concept is from Sergey Ryazanov's "The Impossibly Fast C++ Delegates"
	thread in the codeproject.com

	Currently, there is no type safety in template functions, but it should be
	added.
*/

#include <type_traits>
#include <memory>

namespace polos
{
	template <typename Function> class delegate;

	template<typename Return, typename... Args>
	class delegate<Return(Args...)>
	{
		using return_type = Return;
		using stub_type = return_type(*)(void*, Args&&...);
		using deleter_type = void(*)(void*);

		delegate(void* const object_pointer, stub_type const stub_ptr) noexcept
			: m_ObjectPointer(object_pointer), m_StubPointer(stub_ptr)
		{}
	public:
		delegate() noexcept = default;

		delegate(delegate const&) noexcept = default;

		delegate(delegate&&) noexcept = default;

		delegate(std::nullptr_t const) noexcept : delegate() {}

		template <return_type(* const function_ptr)(Args...)>
		static delegate from() noexcept
		{
			return { nullptr, function_stub<function_ptr> };
		}

		template<class owner_object, return_type(owner_object::* const method_ptr)(Args...)>
		static delegate from(owner_object* const object_pointer) noexcept
		{
			return { object_pointer, method_stub<owner_object, method_ptr> };
		}

		template<class owner_object, return_type(owner_object:: *const method_ptr)(Args...) const>
		static delegate from(owner_object const* const object_pointer) noexcept
		{
			return {object_pointer, const_method_stub<owner_object, method_ptr>};
		}

		static delegate from(return_type(* const function_ptr)(Args&&...))
		{
			return function_ptr;
		}

		bool operator==(delegate const& other) const noexcept
		{
			return (m_ObjectPointer == other.m_ObjectPointer) && (m_StubPointer == other.m_StubPointer);
		}

		bool operator!=(delegate const& other) const noexcept
		{
			return !(*this == other);
		}

		bool operator<(delegate const& other) const noexcept
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

		delegate& operator=(delegate const& other) = default;

		delegate& operator=(delegate&& other) = default;

		template<typename T, std::enable_if_t<!std::is_same_v<delegate, typename std::decay_t<T>>>>
		delegate& operator=(T&& f)
		{
			using type = typename std::decay_t<T>;

			if ((sizeof(type) > m_FunctorPtrSize))
			{
				m_FunctorPtr.reset(operator new(sizeof(type)), functor_deleter<type>);
				m_FunctorPtrSize = sizeof(type);
			}
			else
			{
				m_Deleter(m_FunctorPtr.get());
			}

			new (m_FunctorPtr.get()) type(std::forward<T>(f));

			m_ObjectPointer = m_FunctorPtr.get();
			m_StubPointer   = functor_stub<type>;
			m_Deleter       = deleter_stub<type>;

			return *this;
		}

		auto operator()(Args&&... args) const noexcept
		{
			return m_StubPointer(m_ObjectPointer, std::forward<Args>(args)...);
		}
	private:
		template <typename T>
		static void functor_deleter(void* const p)
		{
			static_cast<T*>(p)->~T();

			operator delete(p);
		}

		template <typename T>
		static void deleter_stub(void* const p)
		{
			static_cast<T*>(p)->~T();
		}

		template <typename T>
		return_type functor_stub(void* const object_ptr, Args&&... args)
		{
			return (*static_cast<T*>(object_ptr))(std::forward<Args>(args)...);
		}

		template <return_type(* function_ptr)(Args...)>
		static return_type function_stub(void* const, Args&&... args)
		{
			return function_ptr(std::forward<Args>(args)...);
		}

		template <typename owner_object, return_type(owner_object::* method_ptr)(Args...)>
		static return_type method_stub(void* const object_ptr, Args&&... args)
		{
			return (static_cast<owner_object*>(object_ptr)->*method_ptr)(std::forward<Args>(args)...);
		}

		template <typename owner_object, return_type(owner_object::* method_ptr)(Args...) const>
		static return_type const_method_stub(void* const object_ptr, Args&&... args)
		{
			return (static_cast<owner_object*>(object_ptr)->*method_ptr)(std::forward<Args>(args)...);
		}

		stub_type m_StubPointer;
		void* m_ObjectPointer;

		deleter_type m_Deleter;
		std::shared_ptr<void> m_FunctorPtr;
		size_t m_FunctorPtrSize;
	};
}

#endif