///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/ecs/component_pool.hpp"

#include <utility>

namespace polos::ecs
{

ComponentPool::~ComponentPool()
{
    Destroy();
}

ComponentPool::ComponentPool(ComponentPool&& tOther)
    : type{tOther.type},
      elemSize(tOther.elemSize),
      mTotalComponents(tOther.mTotalComponents),
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
    std::swap(elemSize, tOther.elemSize);
    std::swap(mTotalComponents, tOther.mTotalComponents);
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
    mTotalComponents = 0U;
    elemSize         = 0U;
    mInitialized     = false;
}

auto ComponentPool::IsInitialized() const -> bool
{
    return mInitialized;
}

auto ComponentPool::GetTotalComponents() const -> std::uint32_t
{
    return mTotalComponents;
}

}// namespace polos::ecs