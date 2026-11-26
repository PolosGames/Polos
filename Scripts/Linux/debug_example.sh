#!/usr/bin/env bash
set -euo pipefail
source "$(dirname -- "${BASH_SOURCE[0]}")/common.sh"

parse_polos_example_args "$@"
build_polos_example "$example" "$preset" "$preprocess"

cd "$polos_root/out/bin/$preset/$example"
exec lldb -o run -- "./$example"
