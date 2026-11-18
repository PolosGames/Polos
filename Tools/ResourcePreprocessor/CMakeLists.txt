cmake_minimum_required(VERSION 3.25)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(ResourcePreprocessor LANGUAGES CXX)

add_executable(ResourcePreprocessor main.cpp)

target_compile_features(ResourcePreprocessor PRIVATE cxx_std_23)
target_link_libraries(ResourcePreprocessor PRIVATE fastgltf::fastgltf)

install(TARGETS ResourcePreprocessor
    RUNTIME DESTINATION "${POLOS_INSTALL_DIR}/tools" COMPONENT SDK)
