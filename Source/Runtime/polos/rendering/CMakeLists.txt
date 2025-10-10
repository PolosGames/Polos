cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

generate_versioning_info(
    NAME rendering
    VERSION_MAJOR 1
    VERSION_MINOR 0
    VERSION_PATCH 0
)

project(
    rendering
        LANGUAGES CXX
        VERSION ${LIB_VERSION_MAJOR}.${LIB_VERSION_MINOR}.${LIB_VERSION_PATCH}
)

define_polos_module(
    NAME
        rendering
    TYPE
        SHARED
    SOURCES
        "src/dll_main.cpp"
        "src/pipeline_cache.cpp"
        "src/render_context.cpp"
        "src/vertex.cpp"
        "src/vk_instance.cpp"
        "src/vulkan_context.cpp"
        "src/vulkan_device.cpp"
        "src/vulkan_resource_manager.cpp"
        "src/vulkan_swapchain.cpp"
    PUBLIC_DEPS
        glfw
        Vulkan::Vulkan
        polos::logging
    PRIVATE_DEPS
        polos::communication
        polos::filesystem
)
