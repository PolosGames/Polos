#if (WIN32)
#    set(CMAKE_C_COMPILER cl.exe)
#    set(CMAKE_CXX_COMPILER cl.exe)
#endif ()
#
#set(CMAKE_EXPORT_COMPILE_COMMANDS True)
#set(BUILD_EDITOR True)

include("$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")