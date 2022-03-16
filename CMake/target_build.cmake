cmake_minimum_required(VERSION 3.21)

function(build_options target)

    if (MSVC)
        target_compile_options(${target} PRIVATE "/W3")
    elseif (CMAKE_C_COMPILER_ID STREQUAL "GNU" OR
            CMAKE_C_COMPILER_ID STREQUAL "Clang" OR
            CMAKE_C_COMPILER_ID STREQUAL "AppleClang")

        target_compile_options(${target} PRIVATE "-Wall")
    endif()

    if(PL_DEBUG)
        if (MSVC)
            target_compile_options(${target} PRIVATE "/Od")
        elseif (CMAKE_C_COMPILER_ID STREQUAL "GNU" OR
                CMAKE_C_COMPILER_ID STREQUAL "Clang" OR
                CMAKE_C_COMPILER_ID STREQUAL "AppleClang")

            target_compile_options(${target} PRIVATE "-O0")
        endif()
    elseif(PL_RELEASE)
        if (MSVC)
            target_compile_options(${target} PRIVATE "/02")
        elseif ($<C_COMPILER_ID:GNU> OR
                $<C_COMPILER_ID:Clang> OR
                $<C_COMPILER_ID:AppleClang>)
            target_compile_options(${target} PRIVATE "-O3")
        endif()
    endif()

endfunction()