cmake_minimum_required(VERSION 3.25)

polos_sources(
    base_live_layer.cpp
    engine_layer.cpp
    engine.cpp
    main_loop.cpp
)

# MainLoop is internal to the runtime library.
polos_tests(core SOURCES test/main_loop_test.cpp main_loop.cpp)
