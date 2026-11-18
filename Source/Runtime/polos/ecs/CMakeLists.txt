cmake_minimum_required(VERSION 3.25)

polos_sources(
    systems/light_collection_system.cpp
    systems/mesh_instance_sync_system.cpp
    systems/editor_camera_system.cpp
    component_pool.cpp
    entity.cpp
    scene_manager.cpp
    scene_graph.cpp
    scene.cpp
    system_registry.cpp
)

polos_tests(scene-graph SOURCES test/scene_graph_test.cpp)

polos_tests(scene-serialization SOURCES test/scene_serialization_test.cpp LABELS ct)
if (BUILD_TESTS)
    target_link_libraries(ut-scene-serialization PRIVATE ryml::ryml)
endif()

polos_tests(ecs SOURCES
    test/component_pool_test.cpp
    test/editor_camera_system_test.cpp
    test/scene_test.cpp
    test/view_test.cpp
)
