# Resource loading

`ResourceManager` owns CPU resources indexed only by `ResourceId`, an alias for Polos's
`utils::string_id`. The engine calls `LoadCatalog()` before initializing rendering.
This reads `Resources/resources.json` and preloads every listed resource. `Resources` is
beside the running executable, independent of the process's working directory.
Build-tree and installed example executables receive the same resource layout.

The catalog is the only registration mechanism. Its `id` is the resource's stable
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

auto const& resources = polos::resource::ResourceManager::Instance();
auto character = resources.GetImage("texture.character"_sid);
auto icon = resources.GetImage("texture.icon"_sid);
```

`GetImage`, `GetModel`, and `GetFile` accept only `ResourceId`. The `_sid` literal hashes
the catalog ID at compile time; `utils::StrHash64(idString)` does the same at runtime.
There are no path, filename, or short-name lookups. Resources with matching filenames
are independent because their IDs differ. Hashing a file path does not retrieve
an resource unless that exact string was explicitly assigned as its catalog ID.

The getters return cached shared pointers without filesystem access, decoding,
or GPU uploads. Unknown IDs, cleared cache entries, and requests for a different
representation return nullptr. For example, `GetFile` returns nullptr for an resource
preloaded as an image; it does not read encoded image bytes on demand.

Add resources to `Source/Resource/resources.json` and package their files under the same
relative paths. Images are decoded, `.gltf`/`.glb` models are parsed, and other
extensions (including `.spv` shaders) are read as raw files. Extension classification
ignores case. Every listed resource must load successfully before rendering starts.
`LoadCatalog()` returns void; existing catalog, validation, and resource-loading
failure checks use `PL_ASSERT`. Debug builds use the standard assertion diagnostic;
Release builds print the condition, reason, and source location to stderr and abort.
Duplicate IDs, ID hash collisions, and duplicate normalized paths are rejected.
Paths normalize separators and redundant components during catalog loading, and
resolved paths are checked against the Resources root. Paths are not runtime
lookup keys and no path/name indexes are retained.

To move or rename a file, change its catalog `path` and keep its `id` unchanged.
Saved references should store the stable ID string or its numeric `ResourceId`.
The runtime does not rename files or rewrite the catalog. Catalog reloads build
a temporary registry and preload all resources before replacing the active state.
Loading failures abort immediately; there is no error return or recovery path.

`ClearCache()` releases the manager's ownership; getters then return nullptr until
`LoadCatalog()` is called again. Shared pointers already held by callers remain
valid through cache clearing and catalog reloads. Registry and cache access must
occur on the engine thread. Low-level image/model decoders are used by catalog
loading and tests; application code retrieves preloaded resources by ID.
