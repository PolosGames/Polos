#!/usr/bin/env bash
set -euo pipefail
source "$(dirname -- "${BASH_SOURCE[0]}")/common.sh"

usage() {
    printf 'Usage: bash %s PROJECT [PRESET]\n' "$0"
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
build_polos_project "$project" "$preset"

cd "$polos_root/out/bin/$preset/$project"
exec "./$project"
