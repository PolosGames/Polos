#!/usr/bin/env bash
set -euo pipefail
source "$(dirname -- "${BASH_SOURCE[0]}")/common.sh"

url='https://gist.githubusercontent.com/sorbatdev/83973b1d409ce5270200c6f6398b0770/raw'
output="$polos_root/Polos.code-workspace"
download=$(mktemp "$polos_root/.Polos.code-workspace.XXXXXX")
trap 'rm -f -- "$download"' EXIT

if command -v curl >/dev/null; then
    curl --fail --show-error --location --output "$download" "$url"
elif command -v wget >/dev/null; then
    wget -O "$download" "$url"
else
    printf 'Install curl or wget to download the workspace file.\n' >&2
    exit 1
fi

mv -- "$download" "$output"
