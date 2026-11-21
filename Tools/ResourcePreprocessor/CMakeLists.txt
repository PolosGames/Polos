add_executable(
    ResourcePreprocessor
        main.cpp
)

target_compile_features(
    ResourcePreprocessor
        PRIVATE cxx_std_23
)
polos_link_print_support(
    ResourcePreprocessor
)
target_link_libraries(
    ResourcePreprocessor
        PRIVATE fastgltf::fastgltf
)

install(TARGETS ResourcePreprocessor
    RUNTIME DESTINATION "${POLOS_INSTALL_DIR}/tools" COMPONENT SDK)
