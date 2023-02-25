cmake_minimum_required(VERSION 3.24)

add_library(glad STATIC ${CMAKE_CURRENT_SOURCE_DIR}/src/glad.c)

target_include_directories(glad PUBLIC include)

build_options(glad false)

#install(TARGETS glad DESTINATION bin)

#install(FILES include/glad/glad.h DESTINATION include/ThirdParty/glad)