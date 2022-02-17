#include "plpch.h"

#include "stack_allocator.h"
#include "mem_utils.h"

namespace polos::memory
{
	StackAllocator::StackAllocator(uint64 size)
		: _buffer_size(size)
	{
		_buffer = static_cast<byte*>(std::malloc(size));
		_bottom = reinterpret_cast<uintptr>(_buffer);
		_top = _bottom;
	}
	
	StackAllocator::~StackAllocator()
	{
		std::free(_buffer);
		_buffer = nullptr;
	}

	void StackAllocator::Pop()
	{
		if (!_buffer || _buffer_size == 0)
		{
			ASSERT_S(0, "Cannot pop because the buffer is empty! (StackAllocator::Pop)");
			return;
		}

		_top = _prev_top;
		
		uintptr prev_element = reinterpret_cast<stack_header*>(_prev_top)->prev_element;

		// Check if the previous element points somewhere else.
		_prev_top = prev_element < _bottom ? _prev_top : prev_element;
	}

	void StackAllocator::ClearTo(uintptr position)
	{
		if (!(_bottom <= position && position < _bottom + _buffer_size))
		{
			ASSERT_S(0, "Out of bounds memory address passed to stack allocator! (StackAllocator::ClearTo)");
			return;
		}

		if (position >= _top)
		{
			ASSERT_S(0, "Passed address is not in the scope of current allocations! (StackAllocator::ClearTo)");
			return;
		}

		_top = position;
		_prev_top = (reinterpret_cast<stack_header*>(_top))->prev_element;
	}

	void StackAllocator::Clear()
	{
		_prev_top = _bottom;
		_top = _bottom;
	}

	uintptr StackAllocator::GetPosition()
	{
		return _prev_top;
	}

	void* StackAllocator::align(uint64 size)
	{
		stack_header* header;

		uint32 header_size = sizeof(stack_header);
		uintptr end_of_element = _top + header_size + size;

		if (end_of_element > _bottom + _buffer_size)
		{
			ASSERT_S(0, "Stack Allocator is out of memory! Returning null. (StackAllocator::align)");
			return nullptr;
		}

		header = reinterpret_cast<stack_header*>(_top);
		header->prev_element = _prev_top;

		void* ptr = reinterpret_cast<void*>(_top + header_size);
		_prev_top = _top;
		_top = MemUtils::AlignForward(end_of_element);

		return ptr;
	}
} // namespace polos