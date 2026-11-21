set(cpm_version 0.43.2)
set(cpm_sha256 49a3bef91ceb65bb66d57255e12d1ffd22abc2f6408fa9fe4534c544a2f232aa)
set(cpm_url "https://github.com/cpm-cmake/CPM.cmake/releases/download/v${cpm_version}/CPM.cmake")
set(cpm_dir "${CMAKE_CURRENT_BINARY_DIR}/cpm")
set(cpm_script "${cpm_dir}/CPM-${cpm_version}.cmake")

if (NOT EXISTS "${cpm_script}")
    file(MAKE_DIRECTORY "${cpm_dir}")
    file(DOWNLOAD "${cpm_url}" "${cpm_script}.part"
        EXPECTED_HASH SHA256=${cpm_sha256}
        STATUS download_status
        TLS_VERIFY ON
    )
    list(GET download_status 0 download_code)
    if (NOT download_code EQUAL 0)
        file(REMOVE "${cpm_script}.part")
        message(FATAL_ERROR "Failed to download CPM: ${download_status}")
    endif()
    file(RENAME "${cpm_script}.part" "${cpm_script}")
endif()

include("${cpm_script}")
