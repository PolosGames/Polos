polos_sources(
    scene_graph.cpp
    scene_manager.cpp
    scene_util.cpp
    scene.cpp
)

polos_tests(
    scene
        SOURCES test/scene_test.cpp
)

polos_tests(
    scene-graph
        SOURCES test/scene_graph_test.cpp
)

polos_tests(
    scene-serialization
        SOURCES test/scene_serialization_test.cpp
        LIBRARIES ryml::ryml
        LABELS ct
)
