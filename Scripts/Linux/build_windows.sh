#!/usr/bin/env bash
set -euo pipefail
source "$(dirname -- "${BASH_SOURCE[0]}")/common.sh"

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

# Use the Windows Vulkan import library and headers installed by vcpkg.
unset VULKAN_SDK
cmake -S "$polos_root" -B "$build_dir" -G Ninja \
    "-DCMAKE_TOOLCHAIN_FILE=$polos_root/CMake/Toolchain.cmake" \
    -DCMAKE_SYSTEM_NAME=Windows -DCMAKE_SYSTEM_PROCESSOR=x86_64 \
    -DCMAKE_C_COMPILER=x86_64-w64-mingw32-gcc \
    -DCMAKE_CXX_COMPILER=x86_64-w64-mingw32-g++ \
    -DCMAKE_RC_COMPILER=x86_64-w64-mingw32-windres \
    -DVCPKG_HOST_TRIPLET=x64-linux -DVCPKG_TARGET_TRIPLET=x64-mingw-static \
    -DVCPKG_APPLOCAL_DEPS=OFF \
    -DCMAKE_BUILD_TYPE=Release "-DCMAKE_INSTALL_PREFIX=$bin_dir" \
    -DPOLOS_INSTALL_DIR:STRING=. -DBUILD_EXAMPLES=ON -DBUILD_TESTS=OFF
cmake --build "$build_dir" --target sandbox
cmake --install "$build_dir" --component Runtime

target_bin=$(dirname -- "$(x86_64-w64-mingw32-g++ -print-prog-name=ld)")
for dll in libstdc++-6.dll libgcc_s_seh-1.dll libwinpthread-1.dll; do
    path=$(x86_64-w64-mingw32-g++ -print-file-name="$dll")
    if [[ ! -f $path ]]; then
        path="$target_bin/$dll"
    fi
    if [[ ! -f $path ]]; then
        printf 'MinGW runtime DLL bulunamadı: %s\n' "$dll" >&2
        exit 1
    fi
    install -m 755 "$path" "$bin_dir/$dll"
done
