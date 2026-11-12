///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_VK_DESCRIPTOR_POOL_HPP
#define POLOS_RENDERING_VK_DESCRIPTOR_POOL_HPP

#include "polos/rendering/vk/descriptor_set.hpp"

#include <vulkan/vulkan.h>

#include <cstdint>
#include <initializer_list>
#include <memory>
#include <span>
#include <vector>

namespace polos::rendering
{
class DescriptorPool
{
public:
    DescriptorPool()                      = default;
    DescriptorPool(DescriptorPool const&) = delete;
    DescriptorPool(DescriptorPool&& tOther);

    DescriptorPool& operator=(DescriptorPool const&) = delete;
    DescriptorPool& operator=(DescriptorPool&& tOther);
    ~DescriptorPool();

    [[nodiscard]] auto GetSet(std::uint32_t tSetIndex, std::uint32_t tFrameSlot = 0U) const
        -> std::weak_ptr<DescriptorSet>;
    [[nodiscard]] auto GetSetHandle(std::uint32_t tSetIndex, std::uint32_t tFrameSlot = 0U) const -> VkDescriptorSet;
    [[nodiscard]] auto GetSetLayoutAtIndex(std::uint32_t tSetIndex) const -> VkDescriptorSetLayout;

    struct BufferBinding
    {
        std::uint32_t binding;
        VkBuffer      buffer;
        VkDeviceSize  offset{0U};
        VkDeviceSize  range{VK_WHOLE_SIZE};
    };

    void UpdateBuffers(
        std::uint32_t                        tSetIndex,
        std::uint32_t                        tFrameSlot,
        std::initializer_list<BufferBinding> tBindings) const;
    void UpdateImage(
        std::uint32_t                tSetIndex,
        std::uint32_t                tBinding,
        std::uint32_t                tArrayElement,
        VkDescriptorImageInfo const& tImage,
        std::uint32_t                tFrameSlot = 0U) const;
private:
    friend class DescriptorPoolBuilder;

    explicit DescriptorPool(
        std::span<VkDescriptorPoolSize const> tPoolSizes,
        uint32_t                              tMaxSets,
        VkDescriptorPoolCreateFlags           tFlags = 0U);

    void createSet(
        std::span<VkDescriptorSetLayoutBinding const> tBindings,
        std::uint32_t                                 tSetCount,
        VkDescriptorSetLayoutCreateFlags              tLayoutFlags,
        void const*                                   tLayoutNext,
        void const*                                   tAllocationNext);

    struct Binding
    {
        std::uint32_t    index;
        VkDescriptorType type;
    };

    struct SetGroup
    {
        VkDescriptorSetLayout                       layout{VK_NULL_HANDLE};
        std::vector<std::shared_ptr<DescriptorSet>> sets;
        std::vector<Binding>                        bindings;
    };

    void reset();

    VkDevice              mDevice{VK_NULL_HANDLE};
    VkDescriptorPool      mPool{VK_NULL_HANDLE};
    std::vector<SetGroup> mSetGroups;
};
}// namespace polos::rendering

#endif// POLOS_RENDERING_VK_DESCRIPTOR_POOL_HPP
