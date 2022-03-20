cmake_minimum_required(VERSION 3.21)

function(build_options target)

    if (MSVC)
        target_compile_options(${target} PRIVATE "/W3")
    elseif (CMAKE_C_COMPILER_ID STREQUAL "GNU" OR
            CMAKE_C_COMPILER_ID STREQUAL "Clang" OR
            CMAKE_C_COMPILER_ID STREQUAL "AppleClang")

        target_compile_options(${target} PRIVATE "-Wall")
    endif()

    if (MSVC)
        target_compile_options(${target}
            PRIVATE $<$<CONFIG:Debug>: /Od /ZI>
                    $<$<CONFIG:RelWithDebInfo>: /0t /ZI>
                    $<$<CONFIG:Release>: /O2>
                    $<$<CONFIG:MinSizeRel>: /O1>
        )
    elseif (CMAKE_C_COMPILER_ID STREQUAL "GNU" OR
            CMAKE_C_COMPILER_ID STREQUAL "Clang" OR
            CMAKE_C_COMPILER_ID STREQUAL "AppleClang")
    
        target_compile_options(${target}
            PRIVATE $<$<CONFIG:Debug>: -O0 -g>
                    $<$<CONFIG:RelWithDebInfo>: -O2 -g>
                    $<$<CONFIG:Release>: -O2>
                    $<$<CONFIG:MinSizeRel>: -O3>
            )
    endif()

endfunction()