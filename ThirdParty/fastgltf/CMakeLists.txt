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
