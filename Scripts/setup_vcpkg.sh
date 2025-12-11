#!/usr/bin/env bash

set -e

VCPKG_DIR="$HOME/vcpkg"

echo "=== vcpkg Setup Script ==="
echo "Installing vcpkg to: $VCPKG_DIR"

# Check if vcpkg directory already exists
if [ -d "$VCPKG_DIR" ]; then
    echo "vcpkg directory already exists at $VCPKG_DIR"
    read -p "Do you want to remove it and reinstall? (y/N): " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        echo "Removing existing vcpkg directory..."
        rm -rf "$VCPKG_DIR"
    else
        echo "Keeping existing vcpkg. Checking for updates..."
        cd "$VCPKG_DIR"
        git pull
        ./bootstrap-vcpkg.sh -disableMetrics
        echo "vcpkg updated successfully!"
        exit 0
    fi
fi

# Clone vcpkg repository
echo "Cloning vcpkg repository..."
git clone https://github.com/Microsoft/vcpkg.git "$VCPKG_DIR"

# Navigate to vcpkg directory
cd "$VCPKG_DIR"

# Bootstrap vcpkg
echo "Bootstrapping vcpkg (with telemetry disabled)..."
./bootstrap-vcpkg.sh -disableMetrics

echo ""
echo "=== vcpkg installation complete! ==="
echo ""
echo "vcpkg installed at: $VCPKG_DIR"
echo ""

