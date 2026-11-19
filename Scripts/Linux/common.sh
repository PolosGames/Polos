#!/usr/bin/env bash

polos_root=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/../.." && pwd)

select_linux_vulkan_sdk() {
    if [[ -n ${VULKAN_SDK:-} || -z ${HOME:-} ]]; then
        return
    fi

    local sdk
    local -a candidates=()
    for sdk in "$HOME"/VulkanSDK/*/x86_64; do
        if [[ -f $sdk/include/vulkan/vulkan.h ]]; then
            candidates+=("$sdk")
        fi
    done
    if (( ${#candidates[@]} )); then
        VULKAN_SDK=$(printf '%s\n' "${candidates[@]}" | sort -V | tail -n 1)
        export VULKAN_SDK
    fi
}

configure_polos_tree() {
    local build_dir=$1
    shift
    local cache="$build_dir/CMakeCache.txt"
    local retired_toolchain="$polos_root/CMake/Toolchain.cmake"
    local -a options=()

    # Refresh old build trees that still point to Polos's removed toolchain.
    if [[ -f $cache && ! -e $retired_toolchain ]]; then
        local cached_toolchain
        cached_toolchain=$(sed -n 's/^CMAKE_TOOLCHAIN_FILE:[^=]*=//p' "$cache")
        if [[ $cached_toolchain == "$retired_toolchain" ]]; then
            options+=(--fresh)
        fi
    fi

    CPM_SOURCE_CACHE=${CPM_SOURCE_CACHE:-"$polos_root/out/cache/cpm"} \
        cmake "${options[@]}" -S "$polos_root" -B "$build_dir" "$@"
}

configure_polos() {
    local preset=$1 build_dir=$2 install_dir=$3 examples=$4 example=${5:-}
    local -a options=()

    select_linux_vulkan_sdk
    if [[ -n ${VULKAN_SDK:-} && -f "$VULKAN_SDK/lib/VulkanLoader/lib/libvulkan.so" ]]; then
        options+=("-DVulkan_LIBRARY=$VULKAN_SDK/lib/VulkanLoader/lib/libvulkan.so")
    fi
    if [[ $examples == OFF ]]; then
        options+=(-DBUILD_TESTS=OFF)
    fi

    configure_polos_tree "$build_dir" --preset "$preset" \
        "-DCMAKE_INSTALL_PREFIX=$install_dir" -DPOLOS_INSTALL_DIR:STRING=. \
        "-DBUILD_EXAMPLES=$examples" "-DPOLOS_EXAMPLE:STRING=$example" "${options[@]}"
}

validate_polos_example() {
    local project=$1
    if [[ ! $project =~ ^[A-Za-z0-9_][A-Za-z0-9_.+-]*$ || ! -f "$polos_root/Examples/$project/CMakeLists.txt" ]]; then
        printf 'Unknown Polos example: %s\n' "$project" >&2
        return 1
    fi
}

preprocess_polos_project() {
    local build_dir=$1 project_dir=$2
    cmake --build "$build_dir" --target ResourcePreprocessor
    "$build_dir/Tools/ResourcePreprocessor/ResourcePreprocessor" "$project_dir"
}

build_polos_project() {
    local project=$1 preset=$2
    local build_dir="$polos_root/out/build/projects/$preset/$project"
    local bin_dir="$polos_root/out/bin/$preset/$project"

    validate_polos_example "$project"
    configure_polos "$preset" "$build_dir" "$bin_dir" ON "$project"
    preprocess_polos_project "$build_dir" "$polos_root/Examples/$project"
    cmake --build "$build_dir" --target install-runtime
}
