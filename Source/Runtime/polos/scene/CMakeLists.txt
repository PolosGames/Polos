polos_sources(
    scene.cpp
    scene_graph.cpp
    scene_manager.cpp
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
