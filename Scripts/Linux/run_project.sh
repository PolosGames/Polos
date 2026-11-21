#!/usr/bin/env bash
set -euo pipefail
source "$(dirname -- "${BASH_SOURCE[0]}")/common.sh"

usage() {
    printf 'Usage: bash %s PROJECT [PRESET]\n' "$0"
    printf 'PROJECT is an example name under Examples, such as sandbox or CatchTheCoin.\n'
    printf 'PRESET defaults to debug-runtime.\n'
}

if [[ ${1:-} == --help || ${1:-} == -h ]]; then
    usage
    exit 0
fi
if (( $# < 1 || $# > 2 )); then
    usage >&2
    exit 1
fi

project=$1
preset=${2:-debug-runtime}
project_dir="$polos_root/Examples/$project"
validate_polos_example "$project"

build_dir="$polos_root/out/build/projects/$preset/$project"
bin_dir="$polos_root/out/bin/$preset/$project"

configure_polos "$preset" "$build_dir" "$bin_dir" ON "$project"
preprocess_polos_project "$build_dir" "$project_dir"
cmake --build "$build_dir" --target install-runtime

cd "$bin_dir"
exec "./$project"
