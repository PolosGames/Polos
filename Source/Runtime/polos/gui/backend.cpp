///
/// Copyright (c) 2026 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/gui/backend.hpp"

#include "polos/platform/platform_manager.hpp"
#include "polos/rendering/cache/texture_cache.hpp"
#include "polos/rendering/common.hpp"
#include "polos/rendering/settings.hpp"
#include "polos/rendering/vk/command_buffer.hpp"
#include "polos/rendering/vk/vulkan_context.hpp"
#include "polos/rendering/vk/vulkan_device.hpp"
#include "polos/utils/assert.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include <type_traits>
#include <unordered_map>

namespace polos::gui::backend
{
namespace
{
constexpr std::uint32_t                                   kImageCapacity{256U};
std::unordered_map<resource::ResourceId, VkDescriptorSet> gImageDescriptors;

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
    auto const& device = rendering::VulkanDevice::Get();
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    auto& io       = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_DockingEnable;

    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();

    // --- 1. Sizing and Spacing (Modern & Tight) ---
    style.WindowPadding    = ImVec2(8.0f, 8.0f);
    style.FramePadding     = ImVec2(5.0f, 3.0f);
    style.CellPadding      = ImVec2(6.0f, 4.0f);
    style.ItemSpacing      = ImVec2(6.0f, 4.0f);
    style.ItemInnerSpacing = ImVec2(6.0f, 4.0f);
    style.ScrollbarSize    = 13.0f;
    style.GrabMinSize      = 10.0f;

    // --- 2. Borders & Rounding ---
    style.WindowBorderSize = 1.0f;
    style.ChildBorderSize  = 1.0f;
    style.PopupBorderSize  = 1.0f;
    style.FrameBorderSize  = 1.0f;

    style.WindowRounding    = 4.0f;
    style.ChildRounding     = 3.0f;
    style.FrameRounding     = 3.0f;
    style.PopupRounding     = 3.0f;
    style.ScrollbarRounding = 9.0f;
    style.GrabRounding      = 3.0f;
    style.TabRounding       = 3.0f;

    auto* window = platform::PlatformManager::Get().GetMainWindow();
    if (window == nullptr || !ImGui_ImplGlfw_InitForVulkan(window, true))
    {
        ImGui::DestroyContext();
        return false;
    }

    ImGui_ImplVulkan_InitInfo info{
        .ApiVersion          = rendering::kVulkanApiVersion,
        .Instance            = rendering::VulkanContext::Get().mInstance,
        .PhysicalDevice      = device.physDevice,
        .Device              = device.logiDevice,
        .QueueFamily         = device.GetGraphicsQueueFamily(),
        .Queue               = device.gfxQueue,
        .DescriptorPool      = VK_NULL_HANDLE,
        .RenderPass          = tRenderPass,
        .MinImageCount       = 2U,
        .ImageCount          = static_cast<std::uint32_t>(rendering::Settings::kMaxFramesInFlight),
        .MSAASamples         = rendering::kSceneSampleCount,
        .PipelineCache       = VK_NULL_HANDLE,
        .Subpass             = 0U,
        .DescriptorPoolSize  = kImageCapacity + IMGUI_IMPL_VULKAN_MINIMUM_IMAGE_SAMPLER_POOL_SIZE,
        .UseDynamicRendering = false,
#if defined(IMGUI_IMPL_VULKAN_HAS_DYNAMIC_RENDERING)// fking annoying imgui, why enable this by default?!
        .PipelineRenderingCreateInfo = {},
#endif
        .Allocator = nullptr,
        .CheckVkResultFn =
            [](VkResult tResult) {
                PL_ASSERT(tResult >= VK_SUCCESS, "Dear ImGui Vulkan backend failed ({})", static_cast<int>(tResult));
            },
        .MinAllocationSize = 1024ULL * 1024ULL,
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

auto RecordDrawData(rendering::CommandBuffer& tCmdBuf) -> void
{
    ImGui::Render();
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), tCmdBuf.GetHandle());
}

auto GetImageTexture(resource::ResourceId tTexture) -> std::uint64_t
{
    auto itr = gImageDescriptors.find(tTexture);
    if (itr == gImageDescriptors.end())
    {
        if (gImageDescriptors.size() >= kImageCapacity)
        {
            return 0U;
        }

        auto const* texture = rendering::TextureCache::Get().GetTexture(tTexture);
        if (texture == nullptr)
        {
            return 0U;
        }

        auto* const descriptor = ImGui_ImplVulkan_AddTexture(texture->sampler, texture->view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        if (descriptor == VK_NULL_HANDLE)
        {
            return 0U;
        }
        itr = gImageDescriptors.emplace(tTexture, descriptor).first;
    }

    return ToTextureId(itr->second);
}

}// namespace polos::gui::backend
