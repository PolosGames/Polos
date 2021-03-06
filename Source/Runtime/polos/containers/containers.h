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
    using Ref = std::shared_ptr<T>;
}

#endif //POLOS_CONTAINERS_H
