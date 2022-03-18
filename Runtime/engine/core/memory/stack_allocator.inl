namespace polos::memory
{
    template<typename T, typename... Args>
    inline T* StackAllocator::Push(Args&&... args)
    {
        PROFILE_FUNC();
        return new (align(sizeof(T))) T(std::forward<Args>(args)...);
    }
    
    template<typename T>
    inline T* StackAllocator::PushArr(uint64 count)
    {
        PROFILE_FUNC();
        return new (align(sizeof(T) * count)) T[count];
    }
}