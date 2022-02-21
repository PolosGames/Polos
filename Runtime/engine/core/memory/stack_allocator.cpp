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
		offset_ = 0;
		prev_offset_ = 0;
		bottom_ = reinterpret_cast<uintptr>(buffer_);
	}
	
	StackAllocator::~StackAllocator()
	{
		std::free(buffer_);
		buffer_ = nullptr;
	}

	void StackAllocator::Pop()
	{
		if (!buffer_ || buffer_size == 0 ||	offset_ == 0)
		{
			ASSERT_S(0, "Cannot pop because the buffer is empty! (StackAllocator::Pop)");
			return;
		}

		size_t prev_offset = reinterpret_cast<stack_header*>(&buffer_[prev_offset_])->prev_offset;

		offset_ = prev_offset_;
		// Check if the previous element points somewhere out of the buffer.
		prev_offset_ = prev_offset;
	}

	void StackAllocator::Clear()
	{
		prev_offset_ = 0;
		offset_ = bottom_;
	}

	void* StackAllocator::align(uint64 size)
	{
		stack_header* header;

		uint32 header_size = sizeof(stack_header);
		uint64 end_of_new_element = offset_ + header_size + size; // the new unaligned offset

		if (end_of_new_element > buffer_size)
		{
			ASSERT_S(0, "Stack Allocator is out of memory! Returning null. (StackAllocator::align)");
			return nullptr;
		}

		header = reinterpret_cast<stack_header*>(bottom_ + offset_);
		header->prev_offset = prev_offset_;
		prev_offset_ = offset_;
		
		void* ptr = &buffer_[offset_ + header_size];
		offset_ = MemUtils::AlignForward(bottom_ + end_of_new_element) - bottom_;

		return ptr;
	}
} // namespace polos