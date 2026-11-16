#!/usr/bin/env bash
set -euo pipefail
source "$(dirname -- "${BASH_SOURCE[0]}")/common.sh"

preset=${1:-linux-debug}
build_dir="$polos_root/out/build/$preset"
bin_dir="$polos_root/out/bin/$preset"

configure_polos "$build_dir" "$bin_dir" ON
cmake --build --preset linux-release --target ResourcePreprocessor
cd $polos_root/out/build/linux-release/Tools/ResourcePreprocessor
exec ./ResourcePreprocessor Examples/sandbox

cd $polos_root
cmake --build --preset $preset
cmake --install "$build_dir" --component Runtime
