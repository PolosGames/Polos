# Resource loading

Polos reads `Resources/resources.yaml` beside the executable after the rendering caches are initialized. Before application scene creation, it loads models, mesh primitives, materials, and the textures referenced by those materials. Package the catalog and its files together, keeping their paths relative to `Resources/`.

## Prepare a project

Place assets under the project's `Resources/` directory. Pass `--preprocess` to an example launcher to regenerate its catalog before building. For a project using an exported SDK, pass the **project root** to `ResourcePreprocessor`.

**Linux**

```bash
/path/to/project/sdk/tools/ResourcePreprocessor /path/to/project
```

**Windows**

```powershell
& "C:/path/to/project/sdk/tools/ResourcePreprocessor.exe" "C:/path/to/project"
```

The tool scans for PNG and JPEG textures and `.gltf` models. It rewrites `Resources/resources.yaml` and generates YAML descriptors under `Resources/Models/` and `Resources/Materials/`. Keep a model's external buffers and images in the locations referenced by its glTF file. The tool expects base-color images referenced by models to be external files.

## YAML file structure

### `Resources/resources.yaml`

The top-level `resources` key contains a list. Each entry has a unique `id` and a `path` relative to `Resources/`. Texture entries point to image files. Model and material entries point to YAML descriptors.

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

The runtime rejects duplicate IDs and stops startup if it cannot read the catalog or load a model, material, or referenced texture. Texture catalog entries provide paths for material loading. Tetris registers its materials in C++ using those paths. [Tetris catalog](../../../../Examples/Tetris/Resources/resources.yaml)

### `Resources/Models/tile.yaml`

```yaml
id: model.tile
path: Models/tile/tile.gltf
materials:
  - id: material.checker
```

Keep model descriptors directly under `Resources/Models/`. The `path` points to the glTF file relative to `Resources/`. In this example, the source file is `Resources/Models/tile/tile.gltf`.

Use the same model ID as in the catalog. List the cataloged material IDs in the order of the glTF file’s `materials` array, since glTF primitives refer to materials by index. Use `materials: []` for a model with no material assignments.

### `Resources/Materials/checker.yaml`

```yaml
id: material.checker
baseColorFactor: [1.0, 1.0, 1.0, 1.0]
baseColorTexture: texture.checker
tilingFactor: 2.0
```

Use the same material ID as in the catalog. `baseColorFactor` contains four numbers in red, green, blue, alpha order and defaults to `[1.0, 1.0, 1.0, 1.0]`.

`baseColorTexture` is the texture's catalog ID. When it is empty or omitted, Polos uses the corresponding glTF material's external base-color image if one is specified. It looks up that image as `texture.<file-stem>`. Without an external image, Polos uses the built-in default texture. Sampler settings for glTF materials come from the glTF file.

Set the optional `tilingFactor` in material YAML to scale both U and V texture coordinates. The loader defaults to `1.0` when the field is omitted. `ResourcePreprocessor` writes `tilingFactor: 1.0` in every generated material descriptor.

## Use resources from C++

Look up resources by ID. Use `_sid` for string literals and `polos::utils::StrHash64(id)` when the ID is known at runtime.

```cpp
#include <polos/rendering/cache/material_cache.hpp>
#include <polos/rendering/cache/mesh_cache.hpp>
#include <polos/rendering/cache/model_cache.hpp>
#include <polos/rendering/cache/texture_cache.hpp>

using namespace polos::rendering;

auto const* model = ModelCache::Get().GetModel("model.tile"_sid);
auto const* material = MaterialCache::Get().GetMaterial("material.checker"_sid);
auto const* texture = TextureCache::Get().GetTexture("texture.checker"_sid);

if (model != nullptr)
{
    for (auto const& mesh : model->meshes)
    {
        for (ResourceId id : mesh.primitives)
        {
            auto const* primitive = MeshCache::Get().GetMesh(id);
            // Use id and primitive->material when creating a mesh renderer component.
        }
    }
}
```

Cache lookups return `nullptr` when an ID is absent. A model keeps its node hierarchy, transforms, and mesh bounds. Its meshes reference uploaded primitives by ID, and materials reference cached textures by ID.

For a material defined in C++, use `ResourceManager::LoadMaterial(id, material, sampler)`. It uploads the referenced texture before adding the material to the cache. The render context owns the rendering caches and the lifetime of their entries.
