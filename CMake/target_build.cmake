cmake_minimum_required(VERSION 3.21)

function(build_options target warnings)
    
    if(${warnings})
        if (MSVC)
            target_compile_options(${target} PRIVATE /W3)
        elseif (CMAKE_C_COMPILER_ID STREQUAL "GNU" OR
                CMAKE_C_COMPILER_ID STREQUAL "Clang" OR
                CMAKE_C_COMPILER_ID STREQUAL "AppleClang")
        
            target_compile_options(${target}
                PRIVATE -Wall -Wextra -Wpedantic -Wno-unused-parameter
                        -Wno-deprecated-volatile)
        endif()
    endif()
    
    if (MSVC)
        target_compile_options(${target}
            PRIVATE $<$<CONFIG:Debug>: /Od /ZI>
                    $<$<CONFIG:RelWithDebInfo>: /Ot /ZI>
                    $<$<CONFIG:Release>: /O2>
                    $<$<CONFIG:MinSizeRel>: /O1>
        )
    elseif (CMAKE_C_COMPILER_ID STREQUAL "GNU" OR
            CMAKE_C_COMPILER_ID STREQUAL "Clang" OR
            CMAKE_C_COMPILER_ID STREQUAL "AppleClang")
    
        target_compile_options(${target}
            PRIVATE $<$<CONFIG:Debug>: -Og>
                    $<$<CONFIG:RelWithDebInfo>: -O2 -g>
                    $<$<CONFIG:Release>: -O2>
                    $<$<CONFIG:MinSizeRel>: -O3>
            )
    endif()

endfunction()