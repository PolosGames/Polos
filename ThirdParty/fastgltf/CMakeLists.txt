include(FetchContent)

set(BUILD_SHARED_LIBS OFF)
set(FASTGLTF_ENABLE_INSTALL OFF)

FetchContent_Declare(fastgltf
    URL https://codeload.github.com/spnda/fastgltf/tar.gz/refs/tags/v1.0.1
    URL_HASH SHA256=8399c5ed2cd90d0aef0704e65c02be59f866a9baf56bdc310cb647a6744ead94
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE)
FetchContent_MakeAvailable(fastgltf)

set_target_properties(fastgltf PROPERTIES
    FOLDER "ThirdParty"
    POSITION_INDEPENDENT_CODE ON)

# Keep warnings from the bundled SIMD sources and headers scoped to those dependencies.
foreach(simd_target IN ITEMS fastgltf_simdjson fastgltf_simdutf)
    if(TARGET ${simd_target})
        set_property(TARGET ${simd_target} PROPERTY SYSTEM ON)
        target_compile_options(${simd_target} PRIVATE
            "$<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wno-undef;-Wno-missing-declarations>")
    endif()
endforeach()

# fastgltf enables only simdutf's Base64 feature, leaving some functions unused.
if(TARGET fastgltf_simdutf)
    target_compile_options(fastgltf_simdutf PRIVATE
        "$<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wno-unused-function>")
endif()
