cmake_minimum_required(VERSION 3.21)
find_program(CLANG_TIDY_EXE "clang-tidy")

if(CLANG_TIDY_EXE)
    message(STATUS "clang-tidy found: ${CLANG_TIDY_EXE}")
    set(CMAKE_CXX_CLANG_TIDY ${CLANG_TIDY_EXE}
        "--checks=*,-llvmlibc-*,-fuchsia-*,-modernize-use-trailing-return-type,-altera-unroll-loops,-cppcoreguidelines-pro-type-union-access,-hicpp-named-parameter,-misc-non-private-member-variables-in-classes,-cppcoreguidelines-non-private-member-variables-in-classes,-cppcoreguidelines-macro-usage,-readability-use-concise-preprocessor-directives,-cppcoreguidelines-avoid-const-or-ref-data-members,-altera-struct-pack-align,-cppcoreguidelines-pro-bounds-constant-array-index,-clang-diagnostic-gnu-zero-variadic-macro-arguments,-misc-include-cleaner,-clang-analyzer-optin.performance.Padding,-llvm-prefer-static-over-anonymous-namespace,-google-build-using-namespace,-bugprone-easily-swappable-parameters,-cppcoreguidelines-pro-type-member-init,-hicpp-use-equals-default,-altera-id-dependent-backward-branch,-performance-trivially-destructible"
        "--format-style=file"
        "--header-filter=^(?!.*ThirdParty).*"
        "--config={CheckOptions: [{key: cppcoreguidelines-special-member-functions.AllowSoleDefaultDtor, value: true}, {key: hicpp-special-member-functions.AllowSoleDefaultDtor, value: true}, {key: readability-magic-numbers.IgnorePowersOf2IntegerValues, value: true}, {key: hicpp-signed-bitwise.IgnorePositiveIntegerLiterals, value: true}]}"
        "-p" "${CMAKE_BINARY_DIR}")
else()
    message(WARNING "clang-tidy not found!")
endif()
