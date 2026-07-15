add_executable(
    rotc

    main.cpp
    lexer/lexer.cpp
    parser/parser.cpp
    util/file.cpp
)

target_include_directories(
    rotc
    PRIVATE
        .
)

target_compile_features(
    rotc
    PRIVATE
        cxx_std_20
)