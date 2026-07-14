add_library(
    rotrt
    STATIC

    src/value.cpp
    src/object.cpp
)


target_include_directories(
    rotrt
    PUBLIC
        include
)


target_compile_features(
    rotrt
    PUBLIC
        cxx_std_20
)