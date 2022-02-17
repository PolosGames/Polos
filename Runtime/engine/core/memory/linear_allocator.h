#pragma once
#ifndef POLOS_CORE_MEMORY_LINEARALLOCATOR_H_
#define POLOS_CORE_MEMORY_LINEARALLOCATOR_H_

namespace polos::memory
{
	class LinearAllocator
	{
	public:
		LinearAllocator() = default;
		LinearAllocator(uint64 size);
		~LinearAllocator();

		template<typename T, typename... Args>
		T* New(Args&&... args);

		template<typename T>
		T* NewArr(uint64 count);


		void Resize(uint64 size);
		void Clear();
	private:
		/// Sets the current top to be aligned to the end of the requested size
		/// \return The previous top
		void* align(uint64 size);

	private:
		byte* _buffer;
		uintptr _top;
		uint64 _buffer_size;
		std::mutex _buffer_mutex;
	};

	template<typename T, typename... Args>
	inline T* LinearAllocator::New(Args&&... args)
	{
		void* ptr = align(sizeof(T));
		return new (ptr) T(std::forward<Args>(args)...);
	}

	template<typename T>
	inline T* LinearAllocator::NewArr(uint64 count)
	{
		uint64 total_size = sizeof(T) * count;

		void* first = align(total_size);
		return new (first) T[count];
	}
} // namespace polos

#endif /* POLOS_CORE_MEMORY_LINEARALLOCATOR_H_ */