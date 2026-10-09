///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/ecs/component_pool.hpp"

#include <utility>

namespace polos::ecs
{

ComponentPool::~ComponentPool()
{ Destroy(); }

auto ComponentPool::IsInitialized() const -> bool
{ return mInitialized; }

ComponentPool::ComponentPool(ComponentPool&& tOther)
    : type{tOther.type},
      mData{tOther.mData},
      mDestroy{tOther.mDestroy},
      mInitialized{tOther.mInitialized}
{
    tOther.mData        = nullptr;
    tOther.mDestroy     = nullptr;
    tOther.mInitialized = false;
}

ComponentPool& ComponentPool::operator=(ComponentPool&& tOther)
{
    if (this == &tOther)
    {
        return *this;
    }

    Destroy();
    std::swap(type, tOther.type);
    std::swap(mData, tOther.mData);
    std::swap(mDestroy, tOther.mDestroy);
    std::swap(mInitialized, tOther.mInitialized);

    tOther.mData        = nullptr;
    tOther.mDestroy     = nullptr;
    tOther.mInitialized = false;

    return *this;
}

auto ComponentPool::Destroy() -> void
{
    if (mData != nullptr && mDestroy != nullptr)
    {
        mDestroy(mData);
        mData    = nullptr;
        mDestroy = nullptr;
    }
    mInitialized = false;
}

}// namespace polos::ecs