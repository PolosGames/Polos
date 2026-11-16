#!/usr/bin/env bash

cmake --build --preset linux-release --target YamlReader
cd out/build/linux-release/Tools/YamlReader
exec ./YamlReader $1
