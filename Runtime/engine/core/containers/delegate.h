#pragma once
#ifndef POLOS_CONTAINERS_DELEGATE_H
#define POLOS_CONTAINERS_DELEGATE_H

/**
	A function delegate class that's heavily influenced and copied from Legion
	Engine's delegate class.

	Main concept is from Sergey Ryazanov's "The Impossibly Fast C++ Delegates"
	thread in the codeproject.com

	Currently, there is no type safety in template functions, but it should be
	added.
*/

#include <type_traits>
#include <utility>

namespace polos
{
	template <typename Function> class delegate;

	template<typename Return, typename... Args>
	class delegate<Return(Args...)>
	{
		using stub_type = Return(*)(void*, Args&&...);

		delegate(void* const object_pointer, stub_type const stub_ptr) noexcept
			: _object_pointer(object_pointer), _stub_pointer(stub_ptr)
			{}
	public:
		delegate() noexcept = default;

		delegate(delegate const&) noexcept = default;

		delegate(delegate&&) noexcept = default;

		delegate(std::nullptr_t const) noexcept : delegate() {}

		template<class owner_object, Return(owner_object::* const method_ptr)(Args...)>
		static delegate from_method(owner_object* const object_pointer) noexcept
		{
			return { object_pointer, method_stub<owner_object, method_ptr> };
		}

		template<class owner_object, Return(owner_object:: *const method_ptr)(Args...) const>
		static delegate from_method(owner_object const* const object_pointer) noexcept
		{
			return {object_pointer, const_method_stub<owner_object, method_ptr>};
		}

		bool operator==(delegate const& other) const noexcept
		{
			return (_object_pointer == other._object_pointer) && (_stub_pointer == other._stub_pointer);
		}

		bool operator!=(delegate const& other) const noexcept
		{
			return !(*this == other);
		}

		bool operator<(delegate const& other) const noexcept
		{
			return (_object_pointer < other._object_pointer) 
				|| ((_object_pointer == other._object_pointer) && (_stub_pointer < other._stub_pointer));
		}

		bool operator==(std::nullptr_t const) const noexcept
		{
			return !_stub_pointer;
		}

		bool operator!=(std::nullptr_t const) const noexcept
		{
			return _stub_pointer;
		}

		delegate& operator=(delegate const& other)
		{
			if (other == *this) return *this;

			_stub_pointer = other._stub_pointer;
			_object_pointer = other._object_pointer;

			return *this;
		}

		delegate& operator=(delegate&& other)
		{
			if (other == *this) return *this;

			_stub_pointer = std::move(other._stub_pointer);
			_object_pointer = std::move(other._object_pointer);

			other._object_pointer = nullptr;
			other._stub_pointer = nullptr;

			return *this;
		}

		auto operator()(Args... args) const noexcept
		{
			return _stub_pointer(_object_pointer, std::forward<Args>(args)...);
		}
	private:
		template <typename owner_object, Return(owner_object::* method_ptr)(Args...)>
		static Return method_stub(void* const object_ptr, Args&&... args)
		{
			return (static_cast<owner_object*>(object_ptr)->*method_ptr)(std::forward<Args>(args)...);
		}

		template <typename owner_object, Return(owner_object::* method_ptr)(Args...) const>
		static Return const_method_stub(void* const object_ptr, Args&&... args)
		{
			return (static_cast<owner_object*>(object_ptr)->*method_ptr)(std::forward<Args>(args)...);
		}

		stub_type _stub_pointer;
		void* _object_pointer;
	};
}

#endif