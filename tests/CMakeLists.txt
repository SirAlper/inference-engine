find_package(GTest CONFIG REQUIRED)
include(GoogleTest)

add_executable(engine_tests
    storage_test.cpp
)

target_link_libraries(engine_tests PRIVATE engine_core engine_warnings GTest::gtest_main)

gtest_discover_tests(engine_tests)
