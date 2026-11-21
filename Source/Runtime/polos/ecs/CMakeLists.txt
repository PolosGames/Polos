polos_sources(
    systems/light_collection_system.cpp
    systems/mesh_instance_sync_system.cpp
    systems/editor_camera_system.cpp
    component_pool.cpp
    entity.cpp
    system_registry.cpp
)

polos_tests(
    ecs
        SOURCES
            test/component_pool_test.cpp
            test/editor_camera_system_test.cpp
            test/view_test.cpp
)
