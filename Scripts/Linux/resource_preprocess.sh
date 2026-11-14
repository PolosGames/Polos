#!/usr/bin/env bash

cmake --build --preset linux-release --target ResourcePreprocessor
cd out/build/linux-release/Tools/ResourcePreprocessor
exec ./ResourcePreprocessor $1
