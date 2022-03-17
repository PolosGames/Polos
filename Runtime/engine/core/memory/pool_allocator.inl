namespace polos::memory
{
    template<typename T>
    PL_MAYBEUNUSED
    TPoolAllocator<T>::TPoolAllocator(size_t count)
    {
        m_PoolAllocator.Initialize(sizeof(T), count);
    }
    
    template<typename T>
    TPoolAllocator<T>::TPoolAllocator(TPoolAllocator&& other) noexcept
            : m_PoolAllocator(std::move(other.m_PoolAllocator))
    {
    }
    
    template<typename T>
    TPoolAllocator<T>& TPoolAllocator<T>::operator=(TPoolAllocator <T>&& rhs) noexcept
    {
        if(rhs.m_PoolAllocator.Data() == this->m_PoolAllocator.Data())
            return *this;
    
        m_PoolAllocator = std::move(rhs.m_PoolAllocator);
        
        return *this;
    }
    
    template<typename T>
    void TPoolAllocator<T>::Initialize(size_t count)
    {
        m_PoolAllocator.Initialize(sizeof(T), count);
    }
    
    template<typename T>
    template<typename ...Args>
    T* TPoolAllocator<T>::New(Args&&... args)
    {
        PROFILE_FUNC();
        return new (m_PoolAllocator.GetNextFree()) T(std::forward<Args>(args)...);
    }
    
    template<typename T>
    void TPoolAllocator<T>::Free(T* ptr)
    {
        PROFILE_FUNC();
        m_PoolAllocator.Free(ptr);
    }
    
    template<typename T>
    size_t TPoolAllocator<T>::Capacity()
    {
        return m_PoolAllocator.Capacity() / sizeof(T);
    }
    
    template<typename T>
    T* TPoolAllocator<T>::Data()
    {
        return static_cast<T*>(m_PoolAllocator.Data());
    }
    
    template<typename T>
    size_t TPoolAllocator<T>::ByteCapacity()
    {
        return m_PoolAllocator.Capacity();
    }
}