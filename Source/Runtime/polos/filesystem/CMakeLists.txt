polos_sources(
    file_manip.cpp
)

polos_tests(
    filesystem
        SOURCES test/file_manip_test.cpp
        DATA test/test_file.test
)
