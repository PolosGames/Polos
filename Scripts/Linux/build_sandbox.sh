#!/usr/bin/env bash
set -euo pipefail
source "$(dirname -- "${BASH_SOURCE[0]}")/common.sh"

preset=${1:-debug-runtime}
build_dir="$polos_root/out/build/$preset"
bin_dir="$polos_root/out/bin/$preset"

configure_polos "$preset" "$build_dir" "$bin_dir" ON
cmake --build "$build_dir" --target ResourcePreprocessor
"$build_dir/Tools/ResourcePreprocessor/ResourcePreprocessor" "$polos_root/Examples/sandbox"
cmake --build "$build_dir" --target install-runtime
