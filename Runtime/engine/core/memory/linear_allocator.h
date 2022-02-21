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
		void* align(uint64 size);

	private:
		byte* buffer_;
		uintptr bottom_;
		uint64 offset_;
		uint64 buffer_size_;
		std::mutex buffer_mutex;
	};

	template<typename T, typename... Args>
	inline T* LinearAllocator::New(Args&&... args)
	{
		//PROFILE_FUNC();
		
		return new (align(sizeof(T))) T(std::forward<Args>(args)...);
	}

	template<typename T>
	inline T* LinearAllocator::NewArr(uint64 count)
	{
		//PROFILE_FUNC();
		return new (align(sizeof(T) * count)) T[count];
	}
} // namespace polos

#endif /* POLOS_CORE_MEMORY_LINEARALLOCATOR_H_ */