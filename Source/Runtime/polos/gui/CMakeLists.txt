target_sources(polos
    PRIVATE gui.cpp backend.cpp backend.hpp
    PUBLIC FILE_SET runtime_headers FILES gui.hpp
)
target_link_libraries(polos PRIVATE imgui::imgui)
