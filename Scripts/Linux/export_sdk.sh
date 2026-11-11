#!/usr/bin/env bash
set -euo pipefail
source "$(dirname -- "${BASH_SOURCE[0]}")/common.sh"

preset=${1:-linux-release}
build_dir="$polos_root/out/build/sdk/$preset"
package_dir=$(mktemp -d)
trap 'rm -rf -- "$package_dir"' EXIT
version=$(<"$polos_root/VERSION")
archive="$polos_root/out/packages/polos-sdk-$version-$preset.zip"

configure_polos "$build_dir" "$polos_root/out/sdk/$preset" OFF
cmake --build "$build_dir" --target polos
cmake --install "$build_dir" --prefix "$package_dir" --component Runtime
cmake --install "$build_dir" --prefix "$package_dir" --component SDK
mkdir -p -- "$(dirname -- "$archive")"
(cd "$package_dir" && cmake -E tar cf "$archive" --format=zip .)
printf '%s\n' "$archive"
