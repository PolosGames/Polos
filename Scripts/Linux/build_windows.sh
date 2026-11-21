#!/usr/bin/env bash
set -euo pipefail
source "$(dirname -- "${BASH_SOURCE[0]}")/common.sh"

usage() {
    printf 'Usage: bash %s [PROJECT]\n' "$0"
    printf 'PROJECT is an example under Examples and defaults to sandbox.\n'
}

if [[ ${1:-} == --help || ${1:-} == -h ]]; then
    usage
    exit 0
fi
if (( $# > 1 )); then
    usage >&2
    exit 1
fi
project=${1:-sandbox}
validate_polos_example "$project"

toolchain=${POLOS_MINGW_ROOT:-}
if [[ -z $toolchain ]] && ! command -v x86_64-w64-mingw32-g++ >/dev/null; then
    toolchain="$HOME/.local/share/polos/toolchains/mingw-gcc/usr"
fi
if [[ -n $toolchain ]]; then
    export PATH="$toolchain/bin:$PATH"
fi
if ! command -v x86_64-w64-mingw32-g++ >/dev/null; then
    echo 'Install x86_64 MinGW-w64 GCC or set POLOS_MINGW_ROOT to its prefix.' >&2
    exit 1
fi

build_dir="$polos_root/out/build/linux-to-win-mingw-release"
bin_dir="$polos_root/out/bin/linux-to-win-mingw-release"
if [[ $project != sandbox ]]; then
    build_dir="$build_dir/$project"
    bin_dir="$bin_dir/$project"
fi

# Use Linux tools for the catalog and shaders.
select_linux_vulkan_sdk
bash "$polos_root/Scripts/Linux/resource_preprocess.sh" "$polos_root/Examples/$project"

configure_polos_tree "$build_dir" -G Ninja \
    "-DCMAKE_TOOLCHAIN_FILE=$polos_root/CMake/MinGW.cmake" \
    -DCMAKE_BUILD_TYPE=Release "-DCMAKE_INSTALL_PREFIX=$bin_dir" \
    -DPOLOS_INSTALL_DIR:STRING=. -DBUILD_EXAMPLES=ON "-DPOLOS_EXAMPLE:STRING=$project" \
    -DBUILD_TESTS=OFF -DENABLE_PROFILING=OFF
cmake --build "$build_dir" --target install-runtime

target_bin=$(dirname -- "$(x86_64-w64-mingw32-g++ -print-prog-name=ld)")
for dll in libstdc++-6.dll libgcc_s_seh-1.dll libwinpthread-1.dll; do
    path=$(x86_64-w64-mingw32-g++ -print-file-name="$dll")
    if [[ ! -f $path ]]; then
        path="$target_bin/$dll"
    fi
    if [[ ! -f $path ]]; then
        printf 'Could not find MinGW runtime DLL: %s\n' "$dll" >&2
        exit 1
    fi
    install -m 755 "$path" "$bin_dir/$dll"
done
