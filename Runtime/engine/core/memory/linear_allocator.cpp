#include <cstdlib>
#include <cstring>
#include <utility>

#include "debug/plassert.h"

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
    
    void LinearAllocator::Initialize(uint64 size)
    {
        iBuffer  = { static_cast<byte*>(std::malloc(size)), size };
        m_Bottom = reinterpret_cast<uintptr>(iBuffer.buffer);
        m_Offset = 0;
    }
    
    void* LinearAllocator::Allocate(uint64 size)
    {
        return align(size);
    }
    
    void LinearAllocator::Resize(uint64 size)
    {
        if (size <= iBuffer.bufferSize)
        {
            m_Offset = size;
            iBuffer.bufferSize = size;
            return;
        }
        void* old_mem = iBuffer.buffer;
        iBuffer.buffer = static_cast<byte*>(std::malloc(size));
        
        if (!iBuffer.buffer)
        {
            ASSERTSTR(0, "Buffer is null! (LinearAllocator::Resize)");
            return;
        }
        
        {
            std::lock_guard<std::mutex> lock(m_BufferMutex);
            std::memcpy(iBuffer.buffer, old_mem, iBuffer.bufferSize);
            std::free(old_mem);
        }
        iBuffer.bufferSize = size;
        
        old_mem = nullptr; // Probably unnecessary lol.
    }
    
    void LinearAllocator::Clear()
    {
        m_Offset = 0;
    }
    
    void* LinearAllocator::align(uint64 size)
    {
        const uint64 new_offset = m_Offset + size;
        
        if (new_offset > iBuffer.bufferSize)
        {
            ASSERTSTR(0, "LinearAllocator is full.");
            return nullptr;
        }
        
        void* ptr = &iBuffer.buffer[m_Offset];
        
        const uintptr curr_ptr     = m_Bottom + m_Offset;
        const uint64  misalignment = curr_ptr & ( MemUtils::kMemoryAlignment - 1 );
        const uint64  padding      = (MemUtils::kMemoryAlignment - misalignment) & (MemUtils::kMemoryAlignment - 1); // if misalignment == 0, padding becomes 16
        
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
}