namespace polos::memory
{
    template<typename T>
    requires IsDefaultConstructable<T>
    inline T* LinearAllocator::New()
    {
        return new (Allocate(sizeof(T))) T();
    }
    
    template<typename T, typename... Args>
    inline T* LinearAllocator::New(Args&&... args)
    {
        PROFILE_FUNC();
        return new (Allocate(sizeof(T))) T(std::forward<Args>(args)...);
    }
    
    template<typename T>
    inline T* LinearAllocator::NewArr(uint64 count)
    {
        PROFILE_FUNC();
        return new (Allocate(sizeof(T) * count)) T[count];
    }
    
    template<typename T>
    inline void LinearAllocator::Delete(T* ptr)
    {
        delete ptr;
    }
    
    template<typename T>
    inline void LinearAllocator::DeleteArr(T* ptr)
    {
        delete[] ptr;
    }
}