#!/bin/bash

# Determine the project directory
POLOS_DIR=$(dirname "$(dirname "$(readlink -f "$0")")")

# Function to display usage
usage() {
    echo "Usage: $0 <preset-name> <stage-to-stop>"
    echo "Available presets:"
    jq -r '.configurePresets[]?.name' "$POLOS_DIR/CMakePresets.json" 2>/dev/null | sort | sed 's/^/  - /'
    exit 1
}

if [ -z "$1" ]; then
    echo "Error: Preset name not provided."
    usage
fi

if ! jq -e ".configurePresets[] | select(.name == \"$1\")" "$POLOS_DIR/CMakePresets.json" >/dev/null 2>&1; then
    echo "Error: Preset '$1' not found in CMakePresets.json."
    usage
fi

cd "$POLOS_DIR" || { echo "Error: Failed to change to directory $POLOS_DIR"; exit 1; }

cmake --preset "$1"
if [ $? -ne 0 ]; then
    echo "Error: CMake configure step failed for preset '$1'."
    exit 1
fi

if [ $2 == "configure" ]; then
    exit 0
fi

cmake --build --preset "$1"
if [ $? -ne 0 ]; then
    echo "Error: CMake build step failed for preset '$1'."
    exit 1
fi

if [ $2 == "build" ]; then
    exit 0
fi

cd "$POLOS_DIR/out/bin/$1" || { echo "Error: Failed to change to directory $POLOS_DIR/out/bin/$1"; exit 1; }

./dummy
if [ $? -ne 0 ]; then
    echo "Error: Execution of './dummy' failed."
    exit 1
fi