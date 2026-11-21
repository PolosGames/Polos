<img src=".github/resources/logo_bw.svg" alt="logo" width="500"/>

# [![GitHub](https://img.shields.io/github/license/PolosGames/Polos)](https://github.com/PolosGames/Polos/blob/master/LICENSE)

A C++ game engine project.

**Roadmap** can be found in [this](https://trello.com/b/frG6m0AQ) link.

**Coding Guidelines** can be found in [Polos Engine C++ Coding Guidelines](https://bit.ly/3YpjIqH).

### Supported Platforms

- Windows x64
- Linux x64

### To Build

- CMake 3.25 or newer, Ninja, and a C++23 compiler.
- Vulkan 1.3 or newer development headers and loader, plus the glslang shader compiler.
- On Linux, the GLFW development dependencies for X11 and Wayland. On Ubuntu,
  install `pkg-config xorg-dev libwayland-dev libxkbcommon-dev wayland-protocols`.
- Internet access for the first configure. CPM downloads pinned dependencies defined
  in `ThirdParty/`. The build scripts share a source cache under `out/cache/cpm`.
  Set `CPM_SOURCE_CACHE` to use a different directory.

The build scripts refresh a build directory that still points to the retired
toolchain. For a manual CMake invocation, use a fresh build directory or configure
the existing directory once with `cmake --fresh`.

### Runtime and versions

The runtime builds as one `polos` shared library, defined in
`Source/Runtime/CMakeLists.txt`. Directory-local CMake files contribute sources;
all runtime headers are accessible across areas. See [Runtime layout](Source/Runtime/README.md).

The root `VERSION` file starts at `0.0.1` and controls the CMake project version,
shared-library version, `POLOS_VERSION*` macros, and engine build information.
Increase it for each versioned change and commit it with that change:

```sh
cmake -P Scripts/bump_version.cmake              # patch, e.g. 0.0.1 -> 0.0.2
cmake -DPART=minor -P Scripts/bump_version.cmake # resets patch to zero
cmake -DPART=major -P Scripts/bump_version.cmake # resets minor and patch
```

Builds keep a separate local counter in the build directory; no-op builds do not
advance it or modify the tracked semantic version.

### Resources

The sandbox loads its catalog, models, and textures from `Resources` beside the
executable. Their source files are in `Examples/sandbox/Resources`. See the
[resource loading guide](Source/Runtime/polos/resource/README.md) for lookup and caching rules.

Engine shaders live in `Source/Resources/Shaders`. The build compiles them to
SPIR-V and writes the generated `.cpp` byte arrays beside `render_compositor.cpp`
in `Source/Runtime/polos/rendering/compositor`. The compositor includes those arrays
directly into the runtime library.

### Sandbox and SDK

Run the scripts below from the repository root. They configure CMake and build with
Ninja. On Windows, use an x64 Visual Studio Developer PowerShell with the Vulkan
SDK installed. On Linux, install Vulkan development files and glslang, or set
`VULKAN_SDK` to a local SDK.

| Task | Linux | Windows |
| --- | --- | --- |
| Build and stage Sandbox | `bash Scripts/Linux/build_sandbox.sh release-runtime` | `./Scripts/Windows/build_sandbox.ps1 -Preset release-runtime` |
| Build and run an example | `bash Scripts/Linux/run_project.sh sandbox` | `./Scripts/Windows/run_project.ps1 -Project sandbox` |
| Build and debug with LLDB | `bash Scripts/Linux/debug_sandbox.sh` | `./Scripts/Windows/debug_sandbox.ps1` |
| Export the SDK | `bash Scripts/Linux/export_sdk.sh release-sdk` | `./Scripts/Windows/export_sdk.ps1` |

Pass an example name from `Examples` to `run_project`. The runner generates its
resource YAML, builds the runtime and selected example, then launches it from
`out/bin/<preset>/<project>`. The preset defaults to `debug-runtime`. For example,
run `bash Scripts/Linux/run_project.sh CatchTheCoin release-runtime` or
`./Scripts/Windows/run_project.ps1 -Project CatchTheCoin -Preset release-runtime`.

The Sandbox build script stages Sandbox in `out/bin/<preset>` with the engine
shared libraries and resources. The SDK defaults to Release
and writes `out/packages/polos-sdk-<version>-<preset>.zip` with headers, libraries,
and Windows import libraries. Link against Polos and place the
engine shared libraries and `Resources` beside the application executable.

To export directly into a project's `sdk/` directory, pass the project path:

```sh
bash Scripts/Linux/export_sdk.sh /path/to/project
bash Scripts/Linux/export_sdk.sh /path/to/project debug-sdk
```

On Windows, use `./Scripts/Windows/export_sdk.ps1 -Destination C:/path/to/project`
and optionally `-Preset debug-sdk`. An existing `sdk/` is replaced after the export
succeeds. Both scripts also produce the ZIP archive. The SDK includes
`tools/ResourcePreprocessor` (`.exe` on Windows). Run it with the project directory
to generate YAML from that project's `Resources/`:

```sh
/path/to/project/sdk/tools/ResourcePreprocessor /path/to/project
```

The **Export Polos SDK** workflow uploads separate Linux and Windows Release
SDK archives. Running applications with the Windows SDK requires a Vulkan 1.3
driver and the x64 MSVC runtime on the destination machine.

### Windows Sandbox from Linux

Run `bash Scripts/Linux/build_windows.sh [PROJECT]` with Ninja, Linux glslang, and an
x86_64 MinGW-w64 GCC toolchain with C++23 support. The cross-build fetches the
Windows Vulkan headers and generates the import library for the target's
`vulkan-1.dll`. If the compiler is outside
`PATH`, set `POLOS_MINGW_ROOT` to the prefix containing its `bin` directory.
The default project is Sandbox. The script stages its Windows executable, DLLs,
and resources in `out/bin/linux-to-win-mingw-release`. Other examples are staged
in a project-named subdirectory. Use the native Windows build for the MSVC SDK ABI.

## AI Usage

This repository's AI tool usage is limited to create Unit/Component tests, README files, CI units, Scripts and some CMake files.
