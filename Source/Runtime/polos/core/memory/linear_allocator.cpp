
#include "linear_allocator.h"

namespace polos
{
    LinearAllocator::LinearAllocator()
        : internalBuffer ({ nullptr, 0 })
        , m_Bottom(0)
        , m_Offset(0)
    {
    }

    LinearAllocator::LinearAllocator(size_t p_Size)
        : internalBuffer({static_cast<byte*>(std::malloc(p_Size)), p_Size})
        , m_Bottom(reinterpret_cast<uintptr>(internalBuffer.buffer))
        , m_Offset(0)
    {
    }

    LinearAllocator::LinearAllocator(LinearAllocator&& p_Other) noexcept
        : internalBuffer(std::exchange(p_Other.internalBuffer, {nullptr, 0}))
        , m_Bottom(std::exchange(p_Other.m_Bottom, 0))
        , m_Offset(std::exchange(p_Other.m_Offset, 0))
    {
    }

    LinearAllocator& LinearAllocator::operator=(LinearAllocator&& p_Rhs) noexcept
    {
        if (&p_Rhs == this) return *this;
        
        internalBuffer  = std::exchange(p_Rhs.internalBuffer, { nullptr, 0 });
        m_Bottom = std::exchange(p_Rhs.m_Bottom, 0);
        m_Offset = std::exchange(p_Rhs.m_Offset, 0);
        
        return *this;
    }
    
    LinearAllocator::~LinearAllocator()
    {
        PROFILE_FUNC();
        if (internalBuffer.buffer != nullptr)
            std::free(internalBuffer.buffer);
        internalBuffer.buffer = nullptr;
    }
    
    void LinearAllocator::Initialize(size_t p_Size)
    {
        size_t aligned_size = p_Size + memory::k_MemoryAlignment - (p_Size & (memory::k_MemoryAlignment - 1));
        internalBuffer             = {static_cast<byte*>(std::malloc(aligned_size)), aligned_size};
        std::memset(internalBuffer.buffer, 0, aligned_size);
        m_Bottom = reinterpret_cast<uintptr>(internalBuffer.buffer);
        m_Offset = 0;
    }

    void* LinearAllocator::Allocate(size_t p_Size)
    {
        const uint64 new_offset = m_Offset + p_Size;

        if (new_offset > internalBuffer.bufferSize)
        {
            LOG_ENGINE_ERROR("[LinearAllocator::Allocate] Buffer is full!");
            return nullptr;
        }

        void* ptr = &internalBuffer.buffer[m_Offset];

        const uintptr curr_ptr     = m_Bottom + new_offset;
        const uint64  misalignment = curr_ptr & (memory::k_MemoryAlignment - 1);
        // if misalignment == 0, padding becomes 16
        const uint64 padding = (memory::k_MemoryAlignment - misalignment) & (memory::k_MemoryAlignment - 1);

        m_Offset = new_offset + padding;
        return ptr;
    }

    void* LinearAllocator::Allocate(size_t size, size_t offset) const
    {
        const uint64 new_offset = offset + size;

        if (new_offset > internalBuffer.bufferSize)
        {
            LOG_ENGINE_ERROR("[LinearAllocator::Align] Buffer is full!");
            return nullptr;
        }

        const uintptr curr_ptr     = m_Bottom + offset;
        const uint64  misalignment = curr_ptr & (memory::k_MemoryAlignment - 1);
        // if misalignment == 0, padding becomes 16
        const uint64 padding = (memory::k_MemoryAlignment - misalignment) & (memory::k_MemoryAlignment - 1);

        void* ptr = &internalBuffer.buffer[new_offset + padding];

        return ptr;
    }

    void LinearAllocator::Resize(uint64 p_Size)
    {
        if (internalBuffer.buffer == nullptr)
        {
            LOG_ENGINE_ERROR("[LinearAllocator::Resize] Buffer was not initialized, so it cannot be resized.");
            return;
        }

        if (p_Size <= internalBuffer.bufferSize)
        {
            LOG_ENGINE_WARN("[LinearAllocator::Resize] Size shrinking is not implemented yet, returning.");
            return;
        }

        void* old_mem  = internalBuffer.buffer;
        internalBuffer.buffer = static_cast<byte*>(std::malloc(p_Size));

        std::memcpy(internalBuffer.buffer, old_mem, internalBuffer.bufferSize);
        std::free(old_mem);

        m_Bottom                   = reinterpret_cast<uintptr>(internalBuffer.buffer);
        uintptr const curr_ptr     = m_Bottom + internalBuffer.bufferSize;
        uint64 const  misalignment = curr_ptr & (memory::k_MemoryAlignment - 1);
        uint64 const  padding      = (memory::k_MemoryAlignment - misalignment) & (memory::k_MemoryAlignment - 1);

        // if misalignment == 0, padding becomes 16
        internalBuffer.bufferSize = p_Size + padding;

        old_mem = nullptr;// Probably unnecessary lol.
    }

    void LinearAllocator::Clear()
    {
        m_Offset = 0;
    }

    byte* LinearAllocator::Data() const
    {
        return internalBuffer.buffer;
    }

    size_t LinearAllocator::Capacity() const
    {
        return internalBuffer.bufferSize;
    }
    
} // namespace polos