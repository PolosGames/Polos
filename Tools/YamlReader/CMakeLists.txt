cmake_minimum_required(VERSION 3.25)

project(YamlReader)

add_executable(YamlReader main.cpp)

target_link_libraries(YamlReader PRIVATE ryml::ryml)
target_compile_features(YamlReader PRIVATE cxx_std_23)
target_include_directories(YamlReader PRIVATE "${POLOS_DIR}/Runtime")
