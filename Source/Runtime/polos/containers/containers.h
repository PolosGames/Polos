#pragma once
#ifndef POLOS_CORE_CONTAINERS_CONTAINERS_H_
#define POLOS_CORE_CONTAINERS_CONTAINERS_H_

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
        std::make_shared<T>(std::forward<Args>(args)...);
    }

    template<typename T>
    using UniquePtr = std::unique_ptr<T>;
    
    template<typename T, typename... Args>
    [[nodiscard]]
    inline UniquePtr<T> MakeUniq(Args&&... args)
    {
        std::make_unique<T>(std::forward<Args>(args)...);
    }

    template<typename T>
    using Optional = std::optional<T>;
}

#endif //POLOS_CONTAINERS_H
