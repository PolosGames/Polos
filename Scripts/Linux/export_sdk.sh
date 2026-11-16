#!/usr/bin/env bash
set -euo pipefail
source "$(dirname -- "${BASH_SOURCE[0]}")/common.sh"

preset=${1:-release-sdk}
build_dir="$polos_root/out/build/sdk/$preset"
package_dir=$(mktemp -d)
trap 'rm -rf -- "$package_dir"' EXIT
version=$(<"$polos_root/VERSION")
archive="$polos_root/out/packages/polos-sdk-$version-$preset.zip"

configure_polos "$preset" "$build_dir" "$package_dir" OFF
cmake --build "$build_dir" --target install-sdk
mkdir -p -- "$(dirname -- "$archive")"
(cd "$package_dir" && cmake -E tar cf "$archive" --format=zip .)
printf '%s\n' "$archive"
