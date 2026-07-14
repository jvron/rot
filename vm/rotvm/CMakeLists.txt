add_executable(
    rotvm
    main.cpp

    vm.cpp
)

target_include_directories(
    rotvm
    PRIVATE
        .
)

target_compile_features(
    rotvm
    PRIVATE
        cxx_std_20
)

target_link_libraries(
    rotvm
    PRIVATE
        rotrt
)