<img src=".github/resources/logo_bw.svg" alt="logo" width="500"/>

# [![GitHub](https://img.shields.io/github/license/PolosGames/Polos)](https://github.com/PolosGames/Polos/blob/master/LICENSE) ![Build](https://img.shields.io/github/actions/workflow/status/PolosGames/Polos/build.yaml?branch=master)

A game engine project that started as a simple learning project and now became one of my biggest projects so far! Therefore I try to bring new features every week.

**Roadmap** can be found in [this](https://trello.com/b/frG6m0AQ) link.

**Coding Guidelines** can be found in [Polos Engine C++ Coding Guidelines](https://bit.ly/3YpjIqH).

### Supported Platforms

- Windows
    - This is because I currently have a on the go solution for my sound engine.

### To Build

You need:
- CMake minimum of 3.24 (You probably can use an older version and be fine with it, but i defaulted to 3.24)
- Any compiler that supports
  - C++20 concepts,
  - constexpr lambdas,
  - nodiscard(with message), likely/unlikely, maybe_unused attributes,
  is fine
- vcpkg (`VCPKG_ROOT` environment variable needs to be readily available.)

### Current Features:

Fully functional Entity Component system:
* Add/Remove entities to the scene
* Add/Remove components to any entity in the scene indirectly.
* Iterate over the components with;
	* A normal SceneView, that allows you to iterate over specified components.
	* A CommonSet SceneView, a CommonSet is a feature I have developed for Polos that allows you to get together a struct that has the entity and it's desired components that are frequently used together. Inside a CommonSet, you can write specific functions that allows you to manipulate an entity's components without ever needing to touch them inside your for loop.

Window system:
* Open/Close windows as pleased inside the engine framework and the editor.

EventBus / Event System:
* Subscribe to any engine/custom event and bind to a member function inside any class.
* Unsubscribe from any engine/custom event and unbind from the member function.
* Create custom events that can be fired from anywhere in the code.

GUI support with  ImGui

Memory management
* Almost all memory inside Polos is managed by custom allocators.

Sound playing

Editor is stil a WIP! So there is little to no progress on the editor.

### Soon:

- A better Render API that supports 3D as well.
- Different graphics API supports (Vulkan, DirectX)
- A sound engine using OpenAL.
- Scene export for using editor as a standalone system for making games

More major/minor features and enhancements can be found in the roadmap
