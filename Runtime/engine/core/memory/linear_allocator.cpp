#include "plpch.h"

#include <cstdlib>

#include "linear_allocator.h"
#include "mem_utils.h"

#include "time/scope_timer.h"

namespace polos::memory
{
	LinearAllocator::LinearAllocator(uint64 size)
		: buffer_size_(size)
	{
		PROFILE_FUNC();
		buffer_ = (byte*)std::malloc(size);
		offset_ = 0;
		bottom_ = reinterpret_cast<uintptr>(buffer_);
	}

	LinearAllocator::~LinearAllocator()
	{
		PROFILE_FUNC();
		std::free(buffer_);
		buffer_ = nullptr;
	}

	void LinearAllocator::Resize(uint64 size)
	{
		if (size <= buffer_size_)
		{
			offset_ = size;
			buffer_size_ = size;
		}
		else
		{
			void* old_mem = buffer_;
			buffer_ = (byte*)std::malloc(size);

			if (!buffer_)
			{
				ASSERT_S(0, "Buffer is null! (LinearAllocator::Resize)");
				return;
			}

			// Need this bc memcpy is not safe, and this looks fancier :)
			// Could've used memmove which is safer.
			std::lock_guard<std::mutex> lock(buffer_mutex);
			std::memcpy(buffer_, old_mem, buffer_size_);
			
			std::free(old_mem);
			old_mem = nullptr; // Probably unnecessary lol.
		}
	}

	void LinearAllocator::Clear()
	{
		offset_ = 0;
	}

	void* LinearAllocator::align(uint64 size)
	{
		uintptr curr_ptr = bottom_ + offset_;
		ptrdiff aligned_offset = MemUtils::AlignForward(curr_ptr) - bottom_;
		uint64 new_offset = aligned_offset + size;
		
		if (new_offset > buffer_size_)
		{
			ASSERT_S(0, "LinearAllocator is full.");
			return nullptr;
		}

		void* ptr = &buffer_[aligned_offset];
		offset_ = new_offset;
		return ptr;
	}
}