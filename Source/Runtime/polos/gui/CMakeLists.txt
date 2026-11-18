target_sources(polos
    PRIVATE backend.cpp
    PUBLIC FILE_SET runtime_headers FILES backend.hpp imgui_config.hpp
)
target_include_directories(polos SYSTEM PRIVATE "${POLOS_IMGUI_SOURCE_DIR}/backends")
target_link_libraries(polos PUBLIC polos::imgui)
