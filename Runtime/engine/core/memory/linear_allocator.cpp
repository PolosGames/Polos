#include "plpch.h"

#include <cstdlib>

#include "linear_allocator.h"
#include "mem_utils.h"

namespace polos::memory
{
	LinearAllocator::LinearAllocator(uint64 size)
		: buffer_size_(size)
	{
		PROFILE_FUNC();
		buffer_ = static_cast<byte*>(std::malloc(size));
		offset_ = 0;
		bottom_ = reinterpret_cast<uintptr>(buffer_);

		if (MemUtils::IsAligned(bottom_ + offset_)) return;

		bottom_ = MemUtils::AlignForward(bottom_);
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
			byte* old_mem = buffer_;
			buffer_ = reinterpret_cast<byte*>(std::malloc(size));

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
		offset_ = MemUtils::AlignForward(curr_ptr) - bottom_;

		if (offset_ + size > buffer_size_)
		{
			ASSERT_S(0, "LinearAllocator is full.");
			return nullptr;
		}

		void* ptr = &buffer_[offset_];
		memset(ptr, 0, size);
		offset_ += size;
		return ptr;
	}
}