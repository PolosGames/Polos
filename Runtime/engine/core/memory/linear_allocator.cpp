#include "plpch.h"

#include <cstdlib>

#include "linear_allocator.h"
#include "mem_utils.h"

namespace polos::memory
{
	LinearAllocator::LinearAllocator(uint64 size)
		: _buffer_size(size)
	{
		_buffer = reinterpret_cast<byte*>(std::malloc(size));
		_top = reinterpret_cast<uintptr>(_buffer);
	}

	LinearAllocator::~LinearAllocator()
	{
		std::free(_buffer);
		_buffer = nullptr;
	}

	void* LinearAllocator::align(uint64 size)
	{
		if (_top + size > (uintptr)_buffer + _buffer_size) return nullptr;
		
		void* ptr = reinterpret_cast<void*>(_top);
		_top = MemUtils::AlignForward(_top + size);
		return ptr;	
	}

	void LinearAllocator::Resize(uint64 size)
	{
		
		if (size <= _buffer_size)
		{
			_top = reinterpret_cast<uintptr>(_buffer) + size;
			_buffer_size = size;
		}
		else
		{
			byte* old_mem = _buffer;
			_buffer = reinterpret_cast<byte*>(std::malloc(size));

			if (!_buffer) return;

			// Need this bc memcpy is not safe, and this looks fancier :)
			// Could've used memmove which is safer.
			std::lock_guard<std::mutex> lock(_buffer_mutex);
			std::memcpy(_buffer, old_mem, _buffer_size);
			
			std::free(old_mem);
			old_mem = nullptr; // Probably unnecessary lol.
		}
	}

	void LinearAllocator::Clear()
	{
		_top = reinterpret_cast<uintptr>(_buffer);
	}
}