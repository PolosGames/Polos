cmake_minimum_required(VERSION 3.25)

polos_sources(
    component_pool.cpp
    entity.cpp
    scene_manager.cpp
    scene_graph.cpp
    scene.cpp
)

polos_tests(scene-graph SOURCES test/scene_graph_test.cpp)

polos_tests(scene-serialization SOURCES test/scene_serialization_test.cpp)

polos_tests(ecs SOURCES
    test/component_pool_test.cpp
    test/scene_test.cpp
    test/view_test.cpp
)
