#include "plpch.h"

#include "stack_allocator.h"
#include "mem_utils.h"

namespace polos::memory
{
	StackAllocator::StackAllocator(uint64 size)
		: buffer_size(size)
	{
		PROFILE_FUNC();
		buffer_ = static_cast<byte*>(std::malloc(size));
		top_ = reinterpret_cast<uintptr>(buffer_);
		bottom_ = top_;

		if (MemUtils::IsAligned(top_)) return;

		bottom_ = MemUtils::AlignForward(bottom_);
		top_ = bottom_;
	}
	
	StackAllocator::~StackAllocator()
	{
		std::free(buffer_);
		buffer_ = nullptr;
	}

	void StackAllocator::Pop()
	{
		if (!buffer_ || buffer_size == 0)
		{
			ASSERT_S(0, "Cannot pop because the buffer is empty! (StackAllocator::Pop)");
			return;
		}

		top_ = prev_top;
		
		uintptr prev_element = reinterpret_cast<stack_header*>(prev_top)->prev_element;

		// Check if the previous element points somewhere else.
		prev_top = prev_element < bottom_ ? prev_top : prev_element;
	}

	void StackAllocator::ClearTo(uintptr position)
	{
		if (!(bottom_ <= position && position < bottom_ + buffer_size))
		{
			ASSERT_S(0, "Out of bounds memory address passed to stack allocator! (StackAllocator::ClearTo)");
			return;
		}

		if (position >= top_)
		{
			ASSERT_S(0, "Passed address is not in the scope of current allocations! (StackAllocator::ClearTo)");
			return;
		}

		top_ = position;
		prev_top = (reinterpret_cast<stack_header*>(top_))->prev_element;
	}

	void StackAllocator::Clear()
	{
		prev_top = bottom_;
		top_ = bottom_;
	}

	uintptr StackAllocator::GetPosition()
	{
		return prev_top;
	}

	void* StackAllocator::align(uint64 size)
	{
		stack_header* header;

		uint32 header_size = sizeof(stack_header);
		uintptr new_top = top_ + header_size + size;

		if (new_top > bottom_ + buffer_size)
		{
			ASSERT_S(0, "Stack Allocator is out of memory! Returning null. (StackAllocator::align)");
			return nullptr;
		}

		header = reinterpret_cast<stack_header*>(top_);
		header->prev_element = prev_top;

		void* ptr = reinterpret_cast<void*>(top_ + header_size);
		prev_top = top_;
		
		if (MemUtils::IsAligned(new_top))
		{
			top_ = new_top;
			return ptr;
		}

		top_ = MemUtils::AlignForward(new_top);

		return ptr;
	}
} // namespace polos