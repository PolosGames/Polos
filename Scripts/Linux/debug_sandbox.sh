#!/usr/bin/env bash
set -euo pipefail
source "$(dirname -- "${BASH_SOURCE[0]}")/common.sh"

preset=${1:-linux-debug}
"$polos_root/Scripts/Linux/build_sandbox.sh" "$preset"
cd "$polos_root/out/bin/$preset"
exec lldb -o run -- ./sandbox
