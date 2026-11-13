///
/// Copyright (c) 2026 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/utils/string_map.hpp"

#include "polos/utils/constexpr_map.hpp"

#include <array>
#include <mutex>
#include <string>
#include <unordered_map>

namespace polos::utils
{
namespace
{

consteval auto StringEntry(std::string_view tString) -> Pair<string_id, std::string_view>
{
    return {.first = StrHash64(tString), .second = tString};
}

constexpr ConstexprMap kKnownStrings{std::array{
    StringEntry("Scene"),
    StringEntry("Scene.slop"),
    StringEntry("SceneRoot"),
    StringEntry("New Entity"),
    StringEntry("Editor Camera"),

    StringEntry("mesh.PolosDefaultCube.part.0"),
    StringEntry("mesh.PolosDefaultQuad.part.0"),
    StringEntry("texture.polos_default"),
    StringEntry("material.polos_default"),
    StringEntry("shader.vert.forward_shading"),
    StringEntry("shader.frag.forward_shading"),
    StringEntry("shader.comp.main_compute"),
    StringEntry("pipeline.cube"),
    StringEntry("model.suzanne"),

    StringEntry("BeginFrame"),
    StringEntry("EndFrame"),
    StringEntry("EngineTerminate"),
    StringEntry("EngineUpdate"),
    StringEntry("KeyPress"),
    StringEntry("KeyRelease"),
    StringEntry("MouseInput"),
    StringEntry("MouseMove"),
    StringEntry("ProcessInput"),
    StringEntry("RenderUpdate"),
    StringEntry("SystemCreated"),
    StringEntry("WindowClose"),
    StringEntry("WindowFocus"),
    StringEntry("WindowFramebufferResize"),
}};

struct StringRegistry
{
    StringRegistry()
    {
        strings.reserve(kKnownStrings.data.size());
        for (auto const& [id, name] : kKnownStrings.data)
        {
            strings.try_emplace(id, name);
        }
    }

    std::mutex                                 mutex;
    std::unordered_map<string_id, std::string> strings;
};

auto Registry() -> StringRegistry&
{
    static StringRegistry registry;
    return registry;
}

}// namespace

auto StrHash64Runtime(std::string_view tString) -> string_id
{
    auto const             strid    = StrHash64(tString);
    auto&                  registry = Registry();
    std::scoped_lock const lock{registry.mutex};
    registry.strings.try_emplace(strid, tString);
    return strid;
}

auto RegisterString(std::string_view tString) -> string_id
{
    return StrHash64Runtime(tString);
}

auto GetStringFromID(string_id tID) -> std::string_view
{
    auto&                  registry = Registry();
    std::scoped_lock const lock{registry.mutex};
    auto const             found = registry.strings.find(tID);
    return found != registry.strings.end() ? std::string_view{found->second} : std::string_view{};
}

}// namespace polos::utils
