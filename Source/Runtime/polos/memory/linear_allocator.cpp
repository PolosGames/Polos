#include "polos/polos_pch.h"

#include "linear_allocator.h"

namespace polos::memory
{
    LinearAllocator::LinearAllocator()
        : iBuffer ({ nullptr, 0 }),
          m_Bottom(0),
          m_Offset(0)
    {
    }

    LinearAllocator::LinearAllocator(uint64 size)
        : iBuffer ({ static_cast<byte*>(std::malloc(size)), size }),
          m_Bottom(reinterpret_cast<uintptr>(iBuffer.buffer)),
          m_Offset(0)
    {
    }
    
    LinearAllocator::LinearAllocator(LinearAllocator&& other) noexcept
        : iBuffer (std::exchange(other.iBuffer, { nullptr, 0 })),
          m_Bottom(std::exchange(other.m_Bottom, 0)),
          m_Offset(std::exchange(other.m_Offset, 0))
    {
    }
    
    LinearAllocator& LinearAllocator::operator=(LinearAllocator&& rhs) noexcept
    {
        if (&rhs == this) return *this;
        
        iBuffer  = std::exchange(rhs.iBuffer, { nullptr, 0 });
        m_Bottom = std::exchange(rhs.m_Bottom, 0);
        m_Offset = std::exchange(rhs.m_Offset, 0);
        
        return *this;
    }
    
    LinearAllocator::~LinearAllocator()
    {
        PROFILE_FUNC();
        if (iBuffer.buffer != nullptr)
            std::free(iBuffer.buffer);
        iBuffer.buffer = nullptr;
    }
    
    void LinearAllocator::Initialize(size_t size)
    {
        size_t aligned_size = size + memory::MemUtils::kMemoryAlignment - (size & (memory::MemUtils::kMemoryAlignment - 1));
        iBuffer  = { static_cast<byte*>(std::malloc(aligned_size)), aligned_size };
        std::memset(iBuffer.buffer, 0, aligned_size);
        m_Bottom = reinterpret_cast<uintptr>(iBuffer.buffer);
        m_Offset = 0;
    }
    
    void LinearAllocator::Resize(size_t size)
    {
        if (size <= iBuffer.bufferSize)
        {
            ASSERTSTR(0, "Cannot shrink the size! (LinearAllocator::Resize)");
            return;
        }
        void* old_mem = iBuffer.buffer;
        iBuffer.buffer = static_cast<byte*>(std::malloc(size));
        
        if (!iBuffer.buffer)
        {
            ASSERTSTR(0, "Buffer is null! (LinearAllocator::Resize)")
            return;
        }
    
        {
            std::lock_guard<std::mutex> lock(m_BufferMutex);
            std::memcpy(iBuffer.buffer, old_mem, iBuffer.bufferSize);
            std::free(old_mem);
            m_Bottom = reinterpret_cast<uintptr>(iBuffer.buffer);
        }
        const uintptr curr_ptr = m_Bottom + iBuffer.bufferSize;
        const uint64  misalignment = curr_ptr & ( MemUtils::kMemoryAlignment - 1 );
        // if misalignment == 0, padding becomes 16
        const uint64  padding      = (MemUtils::kMemoryAlignment - misalignment) & (MemUtils::kMemoryAlignment - 1);
    
        iBuffer.bufferSize = size + padding;
        
        old_mem = nullptr; // Probably unnecessary lol.
    }
    
    void LinearAllocator::Clear()
    {
        m_Offset = 0;
    }
    
    void* LinearAllocator::Allocate(uint64 size)
    {
        const uint64 new_offset = m_Offset + size;
        
        if (new_offset > iBuffer.bufferSize)
        {
            ASSERTSTR(0, "LinearAllocator is full. (LinearAllocator::Allocate)");
            return nullptr;
        }
        
        void* ptr = &iBuffer.buffer[m_Offset];
        
        const uintptr curr_ptr     = m_Bottom + new_offset;
        const uint64  misalignment = curr_ptr & ( MemUtils::kMemoryAlignment - 1 );
        // if misalignment == 0, padding becomes 16
        const uint64  padding      = (MemUtils::kMemoryAlignment - misalignment) & (MemUtils::kMemoryAlignment - 1);
        
        m_Offset = new_offset + padding;
        return ptr;
    }
    
    byte* LinearAllocator::Data() const
    {
        return iBuffer.buffer;
    }
    
    size_t LinearAllocator::Capacity() const
    {
        return iBuffer.bufferSize;
    }
    
    void* LinearAllocator::Align(size_t size, size_t offset) const
    {
        const uint64 new_offset = offset + size;
    
        if (new_offset > iBuffer.bufferSize)
        {
            ASSERTSTR(0, "LinearAllocator is full.");
            return nullptr;
        }
    
        const uintptr curr_ptr     = m_Bottom + offset;
        const uint64  misalignment = curr_ptr & ( MemUtils::kMemoryAlignment - 1 );
        // if misalignment == 0, padding becomes 16
        const uint64  padding      = (MemUtils::kMemoryAlignment - misalignment) & (MemUtils::kMemoryAlignment - 1);
    
        void* ptr = &iBuffer.buffer[new_offset + padding];
    
        return ptr;
    }
    
} // namespace polos