cmake_minimum_required(VERSION 3.24)

macro(build_options target warnings)
    if (CMAKE_C_COMPILER_ID STREQUAL "GNU" OR
        CMAKE_C_COMPILER_ID STREQUAL "Clang" OR
        CMAKE_C_COMPILER_ID STREQUAL "AppleClang")
        target_compile_options(
            ${target}
                PRIVATE
                    $<$<CONFIG:Debug>:-O0 -g>
                    $<$<CONFIG:RelWithDebInfo>:-O2 -g>
                    $<$<CONFIG:Release>:-O2>
                    $<$<CONFIG:MinSizeRel>:-O3>
        )
    endif()

    # if we are compiling a third party library, we don't care about the
    # warnings, so turn them off
    if(NOT ${warnings})
        return()
    endif()

    # THANKS FOR SOME OF THE WARNINGS, JASON! (@lefticus)
    if (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
        target_compile_options(
            ${target}
                PRIVATE
                    "/permissive-"
                    "/W3"
                    "/WX"
                    "/w14101" # unreferenced local variable
                    "/w14189" # local variable is initialized but not referenced
                    "/w14242" # conversion from 'type1' to 'type1', possible loss of data
                    "/w14254" # conversion from 'type1:field_bits' to 'type2:field_bits', possible loss of data
                    "/w14263" # member function does not override any base class virtual member function
                    "/w14264" # no override available for virtual member function from base 'class'; function is hidden
                    "/w14265" # class has virtual functions, but destructor is not virtual instances of this class may not be destructed correctly
                    "/w14266" # no override available for virtual member function from base 'type'; function is hidden
                    "/w14287" # unsigned/negative constant mismatch
                    "/we4289" # loop control variable declared in the for-loop is used outside the for-loop scope
                    "/w14296" # expression is always 'boolean_value'
                    "/w14303" # old style cast
                    "/w14311" # pointer truncation from 'type1' to 'type2'
                    "/w14545" # expression before comma evaluates to a function which is missing an argument list
                    "/w14546" # function call before comma missing argument list
                    "/w14547" # operator before comma has no effect; expected operator with side-effect
                    "/w14549" # operator before comma has no effect; did you intend 'operator'?
                    "/w14555" # expression has no effect; expected expression with side-effect
                    "/w14619" # pragma warning: there is no warning number 'number'
                    "/w14640" # Enable warning on thread un-safe static member initialization
                    "/w14826" # Conversion from 'type1' to 'type_2' is sign-extended. This may cause unexpected runtime behavior.
                    "/w14905" # wide string literal cast to 'LPSTR'
                    "/w14906" # string literal cast to 'LPWSTR'
                    "/w14928" # illegal copy-initialization; more than one user-defined conversion has been implicitly applied
                    "/w15204" # A class with virtual functions has non-virtual trivial destructor.
                    "/wd4201" # ignoring nameless struct/union bc. glm keeps giving it.
                    "/wd4324" # disable padding warnings
        )
        target_compile_options(${target} PRIVATE "/wd5204") # for quill virtual function error, TODO: Remove
    else()
        target_compile_options(
            ${target}
                PRIVATE 
                    "-pedantic"
                    "-Wall"
                    "-Wextra"
                    "-Wshadow"
                    "-Wpedantic"
                    "-Wnon-virtual-dtor"
                    "-Wold-style-cast"
                    "-Wcast-align"
                    "-Wunused"
                    "-Woverloaded-virtual"
                    "-Wconversion"
                    "-Wsign-conversion"
                    "-Wdouble-promotion"
                    "-Wformat=2"
                    "-pedantic-errors"
                    "-Wzero-as-null-pointer-constant"
        )

        # GCC only warnings
        if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
            target_compile_options(
                ${target}
                    PRIVATE
                        "-Wno-volatile" #have to disable volatile warnings because of glm
                        "-Wmisleading-indentation"
                        "-Wduplicated-cond"
                        "-Wduplicated-branches"
                        "-Wlogical-op"
                        "-Wnull-dereference"
                        "-Wuseless-cast"
            )
        else() # NON-GCC warnings
           target_compile_options(
               ${target}
                   PRIVATE
                       "-Wno-deprecated-volatile" #have to disable volatile warnings because of glm
           )
        endif()
    endif()

endmacro()
