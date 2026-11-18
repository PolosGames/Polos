cmake_minimum_required(VERSION 3.25)

set(CMAKE_CXX_CLANG_TIDY "")

add_subdirectory(quill)
add_subdirectory(stb_image)
add_subdirectory(stb_textedit)
add_subdirectory(fastgltf)
add_subdirectory(imgui)

if (ENABLE_PROFILING)
    add_subdirectory(tracy)
endif()

# Append warning suppression after dependencies have set their own warning flags.
function(polos_suppress_dependency_warnings directory)
    get_property(targets DIRECTORY "${directory}" PROPERTY BUILDSYSTEM_TARGETS)
    foreach(target IN LISTS targets)
        get_target_property(type ${target} TYPE)
        if(type MATCHES "^(STATIC_LIBRARY|SHARED_LIBRARY|MODULE_LIBRARY|OBJECT_LIBRARY|EXECUTABLE)$")
            if(MSVC)
                target_compile_options(${target} PRIVATE /w)
            else()
                target_compile_options(${target} PRIVATE -w)
            endif()
        endif()
    endforeach()

    get_property(subdirectories DIRECTORY "${directory}" PROPERTY SUBDIRECTORIES)
    foreach(subdirectory IN LISTS subdirectories)
        polos_suppress_dependency_warnings("${subdirectory}")
    endforeach()
endfunction()

polos_suppress_dependency_warnings("${CMAKE_CURRENT_SOURCE_DIR}")
