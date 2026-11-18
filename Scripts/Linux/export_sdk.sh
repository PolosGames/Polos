#!/usr/bin/env bash
set -euo pipefail
source "$(dirname -- "${BASH_SOURCE[0]}")/common.sh"

usage() {
    printf 'Usage: bash %s [DESTINATION] [PRESET]\n' "$0"
    printf 'Copies the exported SDK to DESTINATION/sdk. PRESET defaults to release-sdk.\n'
    printf 'With no destination, exports only the SDK ZIP.\n'
}

destination=
preset=release-sdk
if [[ ${1:-} == --help || ${1:-} == -h ]]; then
    usage
    exit 0
fi
if (( $# > 2 )); then
    usage >&2
    exit 1
fi
if (( $# == 1 )); then
    case $1 in
        release-sdk|debug-sdk|win-debug|win-release|win-relwithdebinfo|linux-debug|linux-release|linux-relwithdebinfo)
            preset=$1 ;;
        *) destination=$1 ;;
    esac
elif (( $# == 2 )); then
    destination=$1
    preset=$2
fi
if [[ -n $destination ]]; then
    mkdir -p -- "$destination"
    destination=$(cd -- "$destination" && pwd)
fi
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

if [[ -n $destination ]]; then
    # Stage beside sdk so replacement stays on the same filesystem.
    stage_dir=$(mktemp -d "$destination/.polos-sdk.XXXXXXXX")
    cleanup_sdk_stage() {
        if [[ -e $stage_dir/previous && ! -e $destination/sdk ]]; then
            mv -- "$stage_dir/previous" "$destination/sdk"
        fi
        cmake -E rm -rf "$stage_dir" "$package_dir"
    }
    trap cleanup_sdk_stage EXIT
    cmake -E copy_directory "$package_dir" "$stage_dir/sdk"
    if [[ -e $destination/sdk || -L $destination/sdk ]]; then
        mv -- "$destination/sdk" "$stage_dir/previous"
    fi
    mv -- "$stage_dir/sdk" "$destination/sdk"
    printf '%s\n' "$destination/sdk"
fi
