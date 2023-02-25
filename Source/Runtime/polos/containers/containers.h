#pragma once

#include <vector>
#include <unordered_map>

namespace polos
{
    template<typename T>
    using DArray = std::vector<T>;
    
    template<typename Key, typename Value>
    using HashMap = std::unordered_map<Key, Value>;
    
    template<typename T>
    using PmrVector = std::pmr::vector<T>;
    
    template<typename T>
    using SharedPtr = std::shared_ptr<T>;

    template<typename T, typename... Args>
    [[nodiscard]]
    inline SharedPtr<T> MakeShared(Args&&... args)
    {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }

    template<typename T>
    using UniquePtr = std::unique_ptr<T>;
    
    template<typename T, typename... Args>
    [[nodiscard]]
    inline UniquePtr<T> MakeUniq(Args&&... args)
    {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

    template<typename T>
    using WeakPtr = std::weak_ptr<T>;

    template<typename T>
    using Optional = std::optional<T>;
}
