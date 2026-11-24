# Resource loading

Polos reads `Resources/resources.yaml` beside the executable and preloads cataloged textures, materials, and models before rendering starts. Package the catalog and its files together, keeping their paths relative to `Resources/`.

## Prepare a project

Place assets under the project's `Resources/` directory. The example runners generate the catalog before building. For a project using an exported SDK, pass the **project root** to `ResourcePreprocessor`.

**Linux**

```bash
/path/to/project/sdk/tools/ResourcePreprocessor /path/to/project
```

**Windows**

```powershell
& "C:/path/to/project/sdk/tools/ResourcePreprocessor.exe" "C:/path/to/project"
```

The tool scans for PNG and JPEG textures and `.gltf` models. It rewrites `Resources/resources.yaml` and generates YAML descriptors under `Resources/Models/` and `Resources/Materials/`. Keep a model's external buffers and images in the locations referenced by its glTF file. The tool expects base-color images referenced by models to be external files.

## Catalog entries

Each entry has a resource ID and a path. The ID tells Polos which loader to use. Model and material entries point to their YAML descriptors, and a model descriptor points to its glTF source and lists its material IDs in glTF order.

```yaml
resources:
  - id: texture.checker
    path: Textures/checker.png
  - id: material.checker
    path: Materials/checker.yaml
  - id: model.tile
    path: Models/tile.yaml
```

Use `texture.`, `material.`, and `model.` prefixes for those asset types. Generated texture and model IDs use the file stem, so use unique stems within each type. Material IDs come from glTF material names when present, so keep those names unique across the project. In a hand-edited catalog, keep the ID when moving a file and update only its path.

The runtime rejects duplicate IDs and stops startup if it cannot read the catalog or preload a listed texture, material, or model. See the [Tetris catalog](../../../../Examples/Tetris/Resources/resources.yaml) for a texture-only example.

## Use resources from C++

Look up resources by ID. Use `_sid` for string literals and `polos::utils::StrHash64(id)` when the ID is known at runtime.

```cpp
#include <polos/resource/resource_manager.hpp>
#include <polos/utils/string_id.hpp>

auto const& resources = polos::resource::ResourceManager::Get();
auto image = resources.GetImage("texture.checker"_sid);
auto material = resources.GetMaterial("material.checker"_sid);
auto model = resources.GetModel("model.tile"_sid);
```

The getters return cached shared pointers, or `nullptr` when the requested ID and type are absent. Use `LoadImage(id, path)` or `LoadFile(id, path)` to load additional images or raw files explicitly. Relative paths passed to these functions are resolved from the working directory. After loading a raw file, retrieve it with `GetFile(id)`. The resource cache holds CPU data. The renderer manages GPU uploads separately.

`ClearCache()` releases the manager's references while existing shared pointers remain valid. Reloading the catalog replaces the manager's caches.
