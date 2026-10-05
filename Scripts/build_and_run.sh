#!/bin/bash

set -euo pipefail

POLOS_DIR=$(dirname "$(dirname "$(readlink -f "$0")")")

usage() {
    cat <<USAGE
Usage:
  $0 build [preset]       Build a configured preset (default: linux-debug)
  $0 clean                Remove the out/ directory
  $0 run [preset]         Run dummy (default: linux-debug)
  $0 debug [preset]       Run dummy with LLDB (default: linux-debug)
USAGE
}

cd "$POLOS_DIR"

case "${1:-}" in
    build)
        if [ "$#" -gt 2 ] || { [ "$#" -eq 2 ] && [ -z "$2" ]; }; then
            usage >&2
            exit 1
        fi
        exec cmake --build --preset "${2:-linux-debug}"
        ;;
    clean)
        if [ "$#" -ne 1 ]; then
            usage >&2
            exit 1
        fi
        rm -rf -- "$POLOS_DIR/out"
        ;;
    run|debug)
        if [ "$#" -gt 2 ] || { [ "$#" -eq 2 ] && [ -z "$2" ]; }; then
            usage >&2
            exit 1
        fi
        preset=${2:-linux-debug}
        cd "$POLOS_DIR/out/bin/$preset" || {
            echo "Error: Build output not found for preset '$preset'." >&2
            exit 1
        }
        if [ ! -x ./dummy ]; then
            echo "Error: Executable '$PWD/dummy' not found or not executable." >&2
            exit 1
        fi
        if [ "$1" = debug ]; then
            exec lldb -o run -- ./dummy
        fi
        exec ./dummy
        ;;
    -h|--help)
        usage
        ;;
    *)
        usage >&2
        exit 1
        ;;
esac
