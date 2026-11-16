#!/usr/bin/env bash
set -euo pipefail
source "$(dirname -- "${BASH_SOURCE[0]}")/common.sh"

if (( $# < 1 || $# > 2 )); then
    printf 'Usage: %s PROJECT_DIR [PRESET]\n' "$0" >&2
    exit 1
fi

project_dir=$(cd -- "$1" && pwd)
preset=${2:-release-runtime}
build_dir="$polos_root/out/build/tools/$preset"
configure_polos "$preset" "$build_dir" "$polos_root/out/bin/$preset" OFF
cmake --build "$build_dir" --target ResourcePreprocessor
exec "$build_dir/Tools/ResourcePreprocessor/ResourcePreprocessor" "$project_dir"
