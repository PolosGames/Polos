add_executable(
    YamlReader
        main.cpp
)

target_link_libraries(
    YamlReader
        PRIVATE ryml::ryml
)
target_compile_features(
    YamlReader
        PRIVATE cxx_std_23
)
polos_link_print_support(
    YamlReader
)
target_include_directories(
    YamlReader
        PRIVATE "${POLOS_DIR}/Runtime"
)
