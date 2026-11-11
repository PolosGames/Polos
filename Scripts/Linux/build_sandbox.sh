#!/usr/bin/env bash
set -euo pipefail
source "$(dirname -- "${BASH_SOURCE[0]}")/common.sh"

preset=${1:-linux-debug}
build_dir="$polos_root/out/build/$preset"
bin_dir="$polos_root/out/bin/$preset"

configure_polos "$build_dir" "$bin_dir" ON
cmake --build "$build_dir" --target sandbox
cmake --install "$build_dir" --component Runtime
