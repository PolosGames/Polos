# Polos

WIP project.

Roadmap can be found in [this](https://trello.com/b/frG6m0AQ) link.

### To Build

In the root directory where CMakeLists.txt resides, configure with:

```bash
cmake . -DCMAKE_BUILD_TYPE=Release
```

Then build with

```bash
cmake --build build --config Release --target install
```

And that's it. the `--target install` part is optional, but recommended because this will export the executable to the bin folder.

Be sure to set the STATIC_STD on from the root CMakeLists.txt or you can manually put the shared library version of the std in the same folder as the executable.

Compiling with clang is recommended

### Current Features:

- Logging system
- Memory management
- Event Bus (raising events and subscribing / unsubscribing to them)
- A crude window system
- Platform detection
- GUI

### Soon:

- Rendering API
- Different graphics API supports (Vulkan, DirectX)
