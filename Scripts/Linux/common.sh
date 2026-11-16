#!/usr/bin/env bash

polos_root=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/../.." && pwd)

configure_polos() {
    local preset=$1 build_dir=$2 install_dir=$3 examples=$4
    local sdk_default="$HOME/VulkanSDK/1.4.363.0/x86_64"
    local options=()
    if [[ -z ${VULKAN_SDK:-} && -d "$sdk_default" ]]; then
        export VULKAN_SDK="$sdk_default"
    fi
    if [[ -n ${VULKAN_SDK:-} && -f "$VULKAN_SDK/lib/VulkanLoader/lib/libvulkan.so" ]]; then
        options+=("-DVulkan_LIBRARY=$VULKAN_SDK/lib/VulkanLoader/lib/libvulkan.so")
    fi
    if [[ $examples == OFF ]]; then
        options+=(-DBUILD_TESTS=OFF)
    fi
    cmake -S "$polos_root" --preset "$preset" -B "$build_dir" \
        "-DCMAKE_INSTALL_PREFIX=$install_dir" -DPOLOS_INSTALL_DIR:STRING=. \
        "-DBUILD_EXAMPLES=$examples" "${options[@]}"
}
