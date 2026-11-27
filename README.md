# Polos

<img src=".github/resources/logo_bw.svg" alt="Polos" width="360" />

Polos is a C++23 game engine with a Vulkan renderer.

## Features

- **Rendering** with Vulkan 1.3, instanced meshes, GPU frustum culling, and indirect draws.
- **Scenes** with an entity-component system, and YAML save and load.
- **Assets** with glTF model loading and YAML catalogs for textures, materials, and models. [Resource loading guide](Source/Runtime/polos/resource/README.md)
- **Game and debug tools** with keyboard and mouse input, Dear ImGui overlays, and wireframe primitives.
- **Profiling** with optional Tracy instrumentation for CPU and GPU work.

## Supported platforms

- Linux x64
- Windows x64

## Build requirements

- CMake 3.25 or newer, Ninja, and a C++23 compiler.
- Vulkan 1.3 or newer development headers and loader, plus the glslang shader compiler.
- **Linux** needs GLFW's X11 and Wayland build dependencies. Set `VULKAN_SDK` if you use a local Vulkan SDK. The Ubuntu packages used by CI are listed in the [build setup](.github/actions/setup-polos/action.yml).
- **Windows** needs the Vulkan SDK and an x64 Visual Studio Developer PowerShell.
- The first configure needs internet access to download dependencies through CPM.

Run the commands below from the repository root.

## Run an example

**Linux**

```bash
bash Scripts/Linux/run_example.sh --example Tetris
```

**Windows**

```powershell
./Scripts/Windows/run_example.ps1 --example Tetris
```

Use `--preset <name>` to select a CMake preset. The default is `debug-runtime`. Add `--preprocess` to regenerate the resource catalog before building.

On Linux, `bash Scripts/Linux/debug_example.sh --example Tetris` runs the example in LLDB with the same options.

## Export the SDK to another project

**Linux**

```bash
bash Scripts/Linux/export_sdk.sh /path/to/project
```

**Windows**

```powershell
./Scripts/Windows/export_sdk.ps1 -Destination C:/path/to/project
```

The export places a relocatable CMake package in `sdk/lib/cmake/Polos`. Use an
SDK built for your target platform, compiler ABI, and build configuration.

Add this to your game's `CMakeLists.txt`.

```cmake
cmake_minimum_required(VERSION 3.25)
project(MyGame LANGUAGES CXX)

find_package(Polos CONFIG REQUIRED)

polos_app(
    NAME MyGame
    SOURCES app.cpp
)
```

Configure and build from the game’s project directory.

```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH="$PWD/sdk" -DCMAKE_BUILD_TYPE=Release
cmake --build build
cmake --install build --prefix dist
```

`polos_app` links `polos::polos`, stages the engine libraries beside the
executable, and copies a local `Resources/` directory when present. If it
contains shader sources, make `glslang` or `glslangValidator` available to
the build. The install places the executable, engine runtime, and resources
in `bin/` by default. To manage the application target yourself, link
`polos::polos` directly with `target_link_libraries`.

The SDK's entity capacity is available as `Polos_ENTITY_CAPACITY`. Omit
`ENTITY_CAPACITY` when using an SDK, or pass the SDK's value. Set a different
capacity when building Polos and the application together from source.
