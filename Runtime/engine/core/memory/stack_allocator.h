#pragma once
#ifndef POLOS_CORE_MEMORY_STACKALLOCATOR_H_
#define POLOS_CORE_MEMORY_STACKALLOCATOR_H_

#include "linear_allocator.h"

namespace polos::memory
{
	/// 
	/// \brief Another basic allocation method that allows the last element to be removed
	/// out of the allocated memory.
	///
	/// StackAllocator does not promise have the exact amount of space to allocate
	/// elements, even if you do your math correct, because of the headers used by
	/// the allocator to enforce the LIFO principles.
	/// 
	/// If you want to ensure that the specified space for the buffer won't be
	/// polluted with headers, use; LinearAllocator if you'll have different
	/// objects instantiated inside the same buffer; PoolAllocator if you'll be
	/// instantiating objects of the same type.
	/// 
	/// @see PoolAllocator LinearAllocator
	/// 
	class StackAllocator
	{
		struct stack_header
		{
			uintptr prev_element; /// Stores previous element's header's start
		};
	public:
		StackAllocator(uint64 size);
		~StackAllocator();

		template<typename T, typename... Args>
		T* Push(Args&&... args);

		/// 
		/// Creates an array at the end of this stack buffer.
		///
		/// @tparam T The type of the object whose array is going to be created
		/// @param count The amount of T objects that are going to be created
		template<typename T>
		T* PushArr(uint64 count);

		///
		/// Frees the last element in the buffer.
		/// 
		void Pop();

		///
		/// Clears the stack buffer and allows new allocations to be made.
		/// 
		/// This function doesn't actually free the memory that is allocated by
		/// malloc. Instead this function only makes the top pointer just point
		/// to the start of the stack buffer.
		/// 
		void Clear();

		/// 
		/// Clear to the specified position in the buffer.
		///
		/// @param position The position that was acquired by calling GetPosition()
		/// @see GetPosition
		void ClearTo(uintptr position);

		///
		/// Returns the currently added object/array's starting adress
		/// 
		uintptr GetPosition();
	private:
		void* align(uint64 size);
	private:
		byte* buffer_;
		uintptr bottom_;
		uintptr prev_top;
		uintptr top_;
		uint64 buffer_size;
	};

	template<typename T, typename... Args>
	inline T* StackAllocator::Push(Args&&... args)
	{
		return new (align(sizeof(T))) T(std::forward<Args>(args)...);
	}

	template<typename T>
	inline T* StackAllocator::PushArr(uint64 count)
	{
		return new (align(sizeof(T) * count)) T[count];
	}
} // namespace polos

#endif /* POLOS_CORE_MEMORY_STACKALLOCATOR_H_ */