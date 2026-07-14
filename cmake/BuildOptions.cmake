# Compiler warnings
if(ROT_ENABLE_WARNINGS)

    message(STATUS "Compiler warnings enabled")

    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")

        add_compile_options(
            -Wall
            -Wextra
            -Wpedantic
        )

    elseif(MSVC)

        add_compile_options(
            /W4
        )

    endif()

endif()


# Address Sanitizer
if(ROT_ENABLE_ASAN)

    message(STATUS "Address Sanitizer enabled")

    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")

        add_compile_options(
            -fsanitize=address
        )

        add_link_options(
            -fsanitize=address
        )

    elseif(MSVC)

        add_compile_options(
            /fsanitize=address
        )

        add_link_options(
            /fsanitize=address
        )

    endif()

endif()


# Link Time Optimization
if(ROT_ENABLE_LTO)

    include(CheckIPOSupported)

    check_ipo_supported(
        RESULT LTO_SUPPORTED
    )

    if(LTO_SUPPORTED)

        message(STATUS "Link Time Optimization enabled")

        set(
            CMAKE_INTERPROCEDURAL_OPTIMIZATION
            TRUE
        )

    else()

        message(WARNING "LTO requested but not supported")

    endif()

endif()