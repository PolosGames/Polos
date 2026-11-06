///
/// Copyright (c) 2026 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/gui/backend.hpp"

#include "polos/platform/platform_manager.hpp"
#include "polos/rendering/cache/texture_cache.hpp"
#include "polos/rendering/vk/render_context.hpp"
#include "polos/rendering/vk/settings.hpp"
#include "polos/rendering/vk/vulkan_context.hpp"
#include "polos/rendering/vk/vulkan_device.hpp"
#include "polos/utils/assert.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include <limits>
#include <type_traits>
#include <unordered_map>

namespace polos::gui::backend
{
namespace
{
constexpr std::uint32_t                               kImageCapacity{256U};
std::unordered_map<utils::string_id, VkDescriptorSet> gImageDescriptors;

template<typename T>
auto ToTextureId(T tDescriptor) -> std::uint64_t
{
    if constexpr (std::is_pointer_v<T>)
    {
        return reinterpret_cast<std::uintptr_t>(tDescriptor);
    }
    else
    {
        return tDescriptor;
    }
}

}// namespace

auto Initialize(VkRenderPass tRenderPass) -> bool
{
    auto const& device = rendering::RenderContext::GetVulkanDevice();
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    auto& io       = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_DockingEnable;
    ImGui::StyleColorsDark();

    auto* window = platform::PlatformManager::Instance().GetMainWindow();
    if (window == nullptr || !ImGui_ImplGlfw_InitForVulkan(window, true))
    {
        ImGui::DestroyContext();
        return false;
    }

    ImGui_ImplVulkan_InitInfo info{};
    info.ApiVersion     = VK_API_VERSION_1_3;
    info.Instance       = rendering::RenderContext::GetVulkanContext().mInstance;
    info.PhysicalDevice = device.physDevice;
    info.Device         = device.logiDevice;
    info.QueueFamily    = device.GetGraphicsQueueFamily();
    info.Queue          = device.gfxQueue;
    info.RenderPass     = tRenderPass;
    // Backend upload buffers rotate by render submission, independently of swapchain images.
    info.MinImageCount      = 2U;
    info.ImageCount         = static_cast<std::uint32_t>(Settings::kMaxFramesInFlight);
    info.MSAASamples        = VK_SAMPLE_COUNT_1_BIT;
    info.DescriptorPoolSize = kImageCapacity + IMGUI_IMPL_VULKAN_MINIMUM_IMAGE_SAMPLER_POOL_SIZE;
    info.CheckVkResultFn    = [](VkResult tResult) {
        PL_ASSERT(tResult >= VK_SUCCESS, "Dear ImGui Vulkan backend failed ({})", static_cast<int>(tResult));
    };

    if (!ImGui_ImplVulkan_Init(&info))
    {
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        return false;
    }
    return true;
}

auto Shutdown() -> void
{
    if (ImGui::GetCurrentContext() == nullptr)
    {
        return;
    }

    for (auto const& entry : gImageDescriptors)
    {
        ImGui_ImplVulkan_RemoveTexture(entry.second);
    }
    gImageDescriptors.clear();
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

auto BeginFrame() -> void
{
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

auto RecordDrawData(VkCommandBuffer tCommandBuffer) -> void
{
    ImGui::Render();
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), tCommandBuffer);
}

auto GetImageTexture(utils::string_id tTexture) -> std::uint64_t
{
    auto it = gImageDescriptors.find(tTexture);
    if (it == gImageDescriptors.end())
    {
        if (gImageDescriptors.size() >= kImageCapacity)
        {
            return 0U;
        }

        auto const* texture = rendering::TextureCache::GetTexture(tTexture);
        if (texture == nullptr)
        {
            auto const index = rendering::TextureCache::CreateTexture(tTexture, {});
            if (index == std::numeric_limits<std::size_t>::max())
            {
                return 0U;
            }
            texture = rendering::TextureCache::GetTextureAtIndex(index);
        }

        auto const descriptor =
            ImGui_ImplVulkan_AddTexture(texture->sampler, texture->view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        if (descriptor == VK_NULL_HANDLE)
        {
            return 0U;
        }
        it = gImageDescriptors.emplace(tTexture, descriptor).first;
    }

    return ToTextureId(it->second);
}

}// namespace polos::gui::backend
