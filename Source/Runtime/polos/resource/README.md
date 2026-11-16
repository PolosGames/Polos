# Resource loading

`ResourceManager` owns CPU resources indexed only by `ResourceId`, an alias for Polos's
`utils::string_id`. The engine calls `LoadCatalog()` before initializing rendering.
This reads `Resources/resources.json` and preloads every listed resource. `Resources` is
beside the running executable, independent of the process's working directory.
Build-tree and installed example executables receive the same resource layout.

The catalog registers startup resources. Its `id` is the resource's stable
identity; its `path` is used internally to load the file:

```json
[
  { "id": "texture.character", "path": "characters/tux.png" },
  { "id": "texture.icon", "path": "icons/tux.png" }
]
```

```cpp
#include <polos/resource/image.hpp>
#include <polos/resource/resource_manager.hpp>

auto const& resources = polos::resource::ResourceManager::Get();
auto character = resources.GetImage("texture.character"_sid);
auto icon = resources.GetImage("texture.icon"_sid);
```

`GetImage`, `GetModel`, and `GetFile` accept only `ResourceId`. The `_sid` literal hashes
the resource ID at compile time; `utils::StrHash64(idString)` does the same at runtime.
There are no path, filename, or short-name lookups. Resources with matching filenames
are independent because their IDs differ. Hashing a file path does not retrieve
a resource unless that exact string was explicitly assigned as its ID.

The getters return cached shared pointers without filesystem access, decoding,
or GPU uploads. Unknown IDs, cleared cache entries, and requests for a different
representation return nullptr. For example, `GetFile` returns nullptr for an resource
preloaded as an image; it does not read encoded image bytes on demand.

`LoadImage` and `LoadFile` explicitly load resources under a caller-supplied ID,
without requiring a catalog entry. Each accepts either a `std::filesystem::path`
or a `std::span<std::byte const>`:

```cpp
auto& resources = polos::resource::ResourceManager::Get();
auto preview = resources.LoadImage("texture.preview"_sid, std::filesystem::path("preview.png"));
auto file = resources.LoadFile("file.import"_sid, std::filesystem::path("import.bin"));
auto embedded = resources.LoadImage("texture.embedded"_sid, std::as_bytes(std::span(encodedBytes)));
auto memoryFile = resources.LoadFile("file.memory"_sid, std::as_bytes(std::span(rawBytes)));
```

Path overloads use the supplied filesystem path; relative paths use the current
working directory. Memory image input contains encoded PNG, JPEG, TGA, or other
supported image bytes, decoded to owned RGBA8 pixels with the bottom row first.
Memory file input is copied into an owned byte vector; empty input is a valid empty
file, and memory files have empty path/name metadata. Callers can release or modify
their input bytes after loading.

Successful loads are available through `GetImage`/`GetFile`. Loading the same ID and
representation again returns the original cached shared pointer. An explicit load
can add a second representation under an existing ID. Read/decode failures return
nullptr without caching a failed result, so a later call can retry. `ClearCache()`
and catalog reloads release explicitly loaded resources as well as catalog resources.

The glTF loader enables tinygltf's strict, index, and byte-range validation before
resolving accessors. `CMake/vcpkg-ports/tinygltf` pins the validation-capable upstream
revision; the registry's original v3 alpha does not implement these checks. The
loader separately requires loaded buffer data and rejects unsupported
sparse accessors and accessors without a buffer view. Run the focused checks with
`cmake --build out/build/linux-debug --target ut-resource ut-gltf-validation` followed by
`ctest --test-dir out/build/linux-debug -R '^Gltf(Loader|Validation)Test\.' --output-on-failure`.

Each mesh part stores `primitiveVertices` in accessor order. Indexed triangle
lists also store their original index order in `primitiveIndices` as 32-bit
unsigned integers. Non-indexed triangle lists leave `primitiveIndices` empty.
UVs and colors support float and normalized unsigned byte/short accessors. RGB
colors retain alpha 1. The loader ignores unsupported attributes, uses material
defaults for missing materials, and uses the default magenta texture for missing
maps. Mesh parts support triangle lists.

Add engine resources to `Source/Resources/resources.json` and sandbox resources to
`Examples/sandbox/Resources/resources.json`. Package their files under the same relative
paths. Images are decoded, `.gltf`/`.glb` models are parsed, and other
extensions (including `.spv` shaders) are read as raw files. Extension classification
ignores case. Every listed resource must load successfully before rendering starts.
`LoadCatalog()` returns void; existing catalog, validation, and resource-loading
failure checks use `PL_ASSERT`. Debug builds use the standard assertion diagnostic;
Release builds print the condition, reason, and source location to stderr and abort.
Duplicate IDs and ID hash collisions are rejected. Catalog paths are used as written,
relative to the Resources directory. Resource lookup uses IDs.

To move or rename a file, change its catalog `path` and keep its `id` unchanged.
Saved references should store the stable ID string or its numeric `ResourceId`.
The runtime does not rename files or rewrite the catalog. Catalog reloads build
a temporary registry and preload all resources before replacing the active state.
Catalog loading failures abort immediately; there is no error return or recovery path.

`ClearCache()` releases the manager's ownership; getters then return nullptr until
`LoadCatalog()`, `LoadImage()`, or `LoadFile()` loads them again. Shared pointers already
held by callers remain valid through cache clearing and catalog reloads. Registry
and cache access must occur on the engine thread. Low-level image/model decoders are used by catalog
loading and tests; application code uses the manager's explicit loaders or retrieves
cached resources by ID. Run image and manager tests independently of glTF decoding
tests with `cmake --build out/build/linux-debug --target ut-resource-loading`
and `ctest --test-dir out/build/linux-debug -R '^ResourceLoadingTests$' --output-on-failure`.
